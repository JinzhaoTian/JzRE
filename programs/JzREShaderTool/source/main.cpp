/**
 * @file    main.cpp
 * @brief   Offline shader tool for JzRE cooked shader assets
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include <array>
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

namespace {

using Json = nlohmann::json;

struct KeywordSpec {
    std::string name;
    uint32_t    bit = 0;
};

struct StageSpec {
    std::string stage;
    std::string file;
    std::string entryPoint;
    std::string profile;
};

struct VariantSpec {
    uint64_t                                     keywordMask = 0;
    std::string                                  vertexLayout{"default"};
    Json                                         renderState;
    std::unordered_map<std::string, std::string> defines;
};

struct StageArtifacts {
    std::vector<uint8_t> spirv;
    std::vector<uint8_t> dxil;
    std::string          glsl;
    std::string          msl;
    Json                 reflectionLayout;
};

struct BlobChunk {
    uint32_t             id = 0;
    std::vector<uint8_t> data;
};

struct BlobHeader {
    char     magic[4]   = {'J', 'Z', 'S', 'B'};
    uint32_t version    = 1;
    uint32_t chunkCount = 0;
    uint32_t reserved   = 0;
};

struct ChunkHeader {
    uint32_t id     = 0;
    uint32_t offset = 0;
    uint32_t size   = 0;
    uint32_t flags  = 0;
};

std::string Quote(const std::string &value)
{
    std::string out = "\"";
    for (char c : value) {
        if (c == '\\' || c == '"') {
            out.push_back('\\');
        }
        out.push_back(c);
    }
    out.push_back('"');
    return out;
}

bool RunCommand(const std::string &command)
{
    std::cout << "[JzREShaderTool] " << command << std::endl;
    const int result = std::system(command.c_str());
    return result == 0;
}

bool ReadBinary(const std::filesystem::path &path, std::vector<uint8_t> &out)
{
    std::ifstream stream(path, std::ios::binary);
    if (!stream.is_open()) {
        return false;
    }

    stream.seekg(0, std::ios::end);
    const auto size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    if (size <= 0) {
        out.clear();
        return true;
    }

    out.resize(static_cast<std::size_t>(size));
    stream.read(reinterpret_cast<char *>(out.data()), size);
    return stream.good() || stream.eof();
}

bool ReadText(const std::filesystem::path &path, std::string &out)
{
    std::ifstream stream(path);
    if (!stream.is_open()) {
        return false;
    }

    std::stringstream ss;
    ss << stream.rdbuf();
    out = ss.str();
    return true;
}

bool ReadJson(const std::filesystem::path &path, Json &out)
{
    std::ifstream stream(path);
    if (!stream.is_open()) {
        return false;
    }

    try {
        stream >> out;
    } catch (...) {
        return false;
    }

    return true;
}

std::string StageToSpirvCross(const std::string &stage)
{
    if (stage == "Vertex") return "vert";
    if (stage == "Fragment") return "frag";
    if (stage == "Geometry") return "geom";
    if (stage == "TessellationControl") return "tesc";
    if (stage == "TessellationEvaluation") return "tese";
    if (stage == "Compute") return "comp";
    return "vert";
}

std::string Fnv1a64Hex(const std::string &text)
{
    uint64_t hash = 1469598103934665603ull;
    for (unsigned char c : text) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ull;
    }

    std::ostringstream oss;
    oss << std::hex;
    oss.width(16);
    oss.fill('0');
    oss << hash;
    return oss.str();
}

uint32_t ParseArraySize(const Json &entry)
{
    if (entry.contains("array") && entry["array"].is_array()) {
        uint64_t arraySize = 1;
        for (const auto &dim : entry["array"]) {
            if (dim.is_number_unsigned()) {
                arraySize *= std::max<uint32_t>(1U, dim.get<uint32_t>());
            }
        }
        if (arraySize > 0 && arraySize <= 0xFFFFFFFFull) {
            return static_cast<uint32_t>(arraySize);
        }
    }

    if (entry.contains("count") && entry["count"].is_number_unsigned()) {
        return std::max<uint32_t>(1U, entry["count"].get<uint32_t>());
    }

    return 1U;
}

Json BuildReflectionLayoutFromSpirvCross(const Json &reflect)
{
    struct ReflectionResource {
        std::string name;
        std::string type;
        uint32_t    set       = 0;
        uint32_t    binding   = 0;
        uint32_t    arraySize = 1;
    };

    std::vector<ReflectionResource> resources;

    const auto appendResources = [&reflect, &resources](const char *field, const char *typeName, uint32_t defaultSet, uint32_t defaultBinding) {
        if (!reflect.contains(field) || !reflect[field].is_array()) {
            return;
        }

        for (const auto &entry : reflect[field]) {
            if (!entry.is_object()) {
                continue;
            }

            ReflectionResource resource;
            resource.name      = entry.value("name", std::string());
            resource.type      = typeName;
            resource.set       = entry.value("set", defaultSet);
            resource.binding   = entry.value("binding", defaultBinding);
            resource.arraySize = ParseArraySize(entry);
            resources.push_back(std::move(resource));
        }
    };

    appendResources("ubos", "UniformBuffer", 0U, 0U);
    appendResources("ssbos", "StorageBuffer", 0U, 0U);
    appendResources("sampled_images", "SampledTexture", 0U, 0U);
    appendResources("separate_images", "SampledTexture", 0U, 0U);
    appendResources("separate_samplers", "Sampler", 0U, 0U);
    appendResources("storage_images", "StorageTexture", 0U, 0U);
    appendResources("push_constants", "PushConstants", 0xFFFFFFFFU, 0xFFFFFFFFU);

    std::sort(resources.begin(), resources.end(), [](const ReflectionResource &lhs, const ReflectionResource &rhs) {
        return std::tie(lhs.set, lhs.binding, lhs.type, lhs.name) < std::tie(rhs.set, rhs.binding, rhs.type, rhs.name);
    });

    resources.erase(std::unique(resources.begin(), resources.end(), [](const ReflectionResource &lhs, const ReflectionResource &rhs) {
                        return lhs.set == rhs.set && lhs.binding == rhs.binding && lhs.type == rhs.type && lhs.arraySize == rhs.arraySize && lhs.name == rhs.name;
                    }),
                    resources.end());

    Json layout;
    layout["resources"] = Json::array();
    for (const auto &resource : resources) {
        layout["resources"].push_back({
            {"name", resource.name},
            {"type", resource.type},
            {"set", resource.set},
            {"binding", resource.binding},
            {"arraySize", resource.arraySize},
        });
    }

    return layout;
}

bool WriteBlob(const std::filesystem::path &path, const std::vector<BlobChunk> &chunks)
{
    BlobHeader header{};
    header.chunkCount = static_cast<uint32_t>(chunks.size());

    std::vector<ChunkHeader> table;
    table.reserve(chunks.size());

    uint32_t offset = static_cast<uint32_t>(sizeof(BlobHeader) + sizeof(ChunkHeader) * chunks.size());
    for (const auto &chunk : chunks) {
        ChunkHeader ch{};
        ch.id     = chunk.id;
        ch.offset = offset;
        ch.size   = static_cast<uint32_t>(chunk.data.size());
        table.push_back(ch);
        offset += ch.size;
    }

    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out.is_open()) {
        return false;
    }

    out.write(reinterpret_cast<const char *>(&header), sizeof(header));
    out.write(reinterpret_cast<const char *>(table.data()), static_cast<std::streamsize>(table.size() * sizeof(ChunkHeader)));

    for (const auto &chunk : chunks) {
        if (!chunk.data.empty()) {
            out.write(reinterpret_cast<const char *>(chunk.data.data()), static_cast<std::streamsize>(chunk.data.size()));
        }
    }

    return out.good();
}

std::unordered_map<std::string, std::string> BuildDefines(const VariantSpec &variant, const std::vector<KeywordSpec> &keywords)
{
    if (!variant.defines.empty()) {
        return variant.defines;
    }

    std::unordered_map<std::string, std::string> defines;
    defines.reserve(keywords.size());

    for (const auto &keyword : keywords) {
        const bool enabled    = keyword.bit < 64 && ((variant.keywordMask & (1ull << keyword.bit)) != 0);
        defines[keyword.name] = enabled ? "1" : "0";
    }

    return defines;
}

bool CompileStage(const StageSpec                                    &stage,
                  const std::unordered_map<std::string, std::string> &defines,
                  const std::vector<std::filesystem::path>           &includeDirs,
                  const std::filesystem::path                        &workspaceRoot,
                  const std::filesystem::path                        &tmpDir,
                  StageArtifacts                                     &outArtifacts)
{
    const auto sourceFile = (workspaceRoot / stage.file).lexically_normal();
    const auto stageTag   = StageToSpirvCross(stage.stage);

    const auto spirvPath   = (tmpDir / (stageTag + "_" + stage.entryPoint + ".spv")).lexically_normal();
    const auto dxilPath    = (tmpDir / (stageTag + "_" + stage.entryPoint + ".dxil")).lexically_normal();
    const auto glslPath    = (tmpDir / (stageTag + "_" + stage.entryPoint + ".glsl")).lexically_normal();
    const auto mslPath     = (tmpDir / (stageTag + "_" + stage.entryPoint + ".msl")).lexically_normal();
    const auto reflectPath = (tmpDir / (stageTag + "_" + stage.entryPoint + ".reflect.json")).lexically_normal();

    std::ostringstream dxcSpv;
    dxcSpv << "dxc"
           << " -T " << stage.profile
           << " -E " << stage.entryPoint
           << " -spirv"
           << " -fspv-target-env=vulkan1.2"
           << " -Fo " << Quote(spirvPath.string());

    for (const auto &[name, value] : defines) {
        dxcSpv << " -D" << name << "=" << value;
    }
    for (const auto &includeDir : includeDirs) {
        dxcSpv << " -I " << Quote(includeDir.string());
    }
    dxcSpv << " " << Quote(sourceFile.string());

    if (!RunCommand(dxcSpv.str())) {
        return false;
    }

    std::ostringstream dxcDxil;
    dxcDxil << "dxc"
            << " -T " << stage.profile
            << " -E " << stage.entryPoint
            << " -Fo " << Quote(dxilPath.string());

    for (const auto &[name, value] : defines) {
        dxcDxil << " -D" << name << "=" << value;
    }
    for (const auto &includeDir : includeDirs) {
        dxcDxil << " -I " << Quote(includeDir.string());
    }
    dxcDxil << " " << Quote(sourceFile.string());

    if (!RunCommand(dxcDxil.str())) {
        return false;
    }

    std::ostringstream crossGlsl;
    crossGlsl << "spirv-cross " << Quote(spirvPath.string())
              << " --entry " << stage.entryPoint
              << " --stage " << stageTag
              << " --version 330"
              << " --no-es"
              << " --glsl-emit-ubo-as-plain-uniforms"
              << " --fixup-clipspace"
              << " --output " << Quote(glslPath.string());

    if (!RunCommand(crossGlsl.str())) {
        return false;
    }

    std::ostringstream crossMsl;
    crossMsl << "spirv-cross " << Quote(spirvPath.string())
             << " --entry " << stage.entryPoint
             << " --stage " << stageTag
             << " --msl"
             << " --msl-version 20300"
             << " --output " << Quote(mslPath.string());

    if (!RunCommand(crossMsl.str())) {
        return false;
    }

    std::ostringstream crossReflect;
    crossReflect << "spirv-cross " << Quote(spirvPath.string())
                 << " --entry " << stage.entryPoint
                 << " --stage " << stageTag
                 << " --reflect"
                 << " --output " << Quote(reflectPath.string());

    if (!RunCommand(crossReflect.str())) {
        return false;
    }

    if (!ReadBinary(spirvPath, outArtifacts.spirv)) {
        return false;
    }
    if (!ReadBinary(dxilPath, outArtifacts.dxil)) {
        return false;
    }
    if (!ReadText(glslPath, outArtifacts.glsl)) {
        return false;
    }
    if (!ReadText(mslPath, outArtifacts.msl)) {
        return false;
    }

    Json reflectionJson;
    if (!ReadJson(reflectPath, reflectionJson)) {
        return false;
    }
    outArtifacts.reflectionLayout = BuildReflectionLayoutFromSpirvCross(reflectionJson);

    return true;
}

int RunCook(const std::filesystem::path &inputPath, const std::filesystem::path &outputDir)
{
    std::ifstream input(inputPath);
    if (!input.is_open()) {
        std::cerr << "Failed to open input manifest: " << inputPath << std::endl;
        return 1;
    }

    Json source;
    input >> source;

    if (!source.contains("shaderName") || !source.contains("stages")) {
        std::cerr << "Source manifest must include shaderName and stages" << std::endl;
        return 1;
    }

    const std::string shaderName = source["shaderName"].get<std::string>();

    std::vector<KeywordSpec> keywords;
    if (source.contains("keywords") && source["keywords"].is_array()) {
        uint32_t              implicitBit = 0;
        std::set<uint32_t>    usedBits;
        std::set<std::string> usedNames;
        for (const auto &entry : source["keywords"]) {
            KeywordSpec keyword;
            if (entry.is_string()) {
                keyword.name = entry.get<std::string>();
                keyword.bit  = implicitBit;
            } else {
                keyword.name = entry.value("name", "");
                keyword.bit  = entry.value("bit", implicitBit);
            }

            if (keyword.name.empty()) {
                std::cerr << "Keyword name cannot be empty" << std::endl;
                return 1;
            }

            if (keyword.bit >= 64) {
                std::cerr << "Keyword bit out of range [0, 63]: " << keyword.name
                          << " -> " << keyword.bit << std::endl;
                return 1;
            }

            if (!usedBits.insert(keyword.bit).second) {
                std::cerr << "Duplicate keyword bit index: " << keyword.bit << std::endl;
                return 1;
            }

            if (!usedNames.insert(keyword.name).second) {
                std::cerr << "Duplicate keyword name: " << keyword.name << std::endl;
                return 1;
            }

            keywords.push_back(keyword);
            implicitBit = std::max<uint32_t>(implicitBit + 1, keyword.bit + 1);
        }
    }

    std::map<std::string, StageSpec> stageSpecs;
    for (auto it = source["stages"].begin(); it != source["stages"].end(); ++it) {
        if (!it.value().is_object()) {
            continue;
        }

        StageSpec spec;
        spec.stage      = it.key();
        spec.file       = it.value().value("file", "");
        spec.entryPoint = it.value().value("entryPoint", "main");
        spec.profile    = it.value().value(
            "profile",
            spec.stage == "Fragment" ? "ps_6_6" : (spec.stage == "Compute" ? "cs_6_6" : "vs_6_6"));

        if (spec.file.empty()) {
            std::cerr << "Stage '" << spec.stage << "' missing source file" << std::endl;
            return 1;
        }

        if (spec.entryPoint.empty()) {
            std::cerr << "Stage '" << spec.stage << "' missing entryPoint" << std::endl;
            return 1;
        }

        if (spec.profile.empty()) {
            std::cerr << "Stage '" << spec.stage << "' missing profile" << std::endl;
            return 1;
        }

        stageSpecs[spec.stage] = spec;
    }

    if (stageSpecs.empty()) {
        std::cerr << "No valid stage definitions in source manifest" << std::endl;
        return 1;
    }

    std::vector<VariantSpec> variants;
    if (source.contains("variants") && source["variants"].is_array()) {
        for (const auto &variantNode : source["variants"]) {
            if (!variantNode.is_object()) {
                continue;
            }

            VariantSpec variant;
            variant.keywordMask  = variantNode.value("keywordMask", 0ull);
            variant.vertexLayout = variantNode.value("vertexLayout", "default");

            if (variantNode.contains("renderState")) {
                variant.renderState = variantNode["renderState"];
            }

            if (variantNode.contains("defines") && variantNode["defines"].is_object()) {
                for (auto d = variantNode["defines"].begin(); d != variantNode["defines"].end(); ++d) {
                    if (d.value().is_string()) {
                        variant.defines[d.key()] = d.value().get<std::string>();
                    } else if (d.value().is_number_integer()) {
                        variant.defines[d.key()] = std::to_string(d.value().get<int>());
                    } else if (d.value().is_boolean()) {
                        variant.defines[d.key()] = d.value().get<bool>() ? "1" : "0";
                    }
                }
            }

            variants.push_back(std::move(variant));
        }
    }

    if (variants.empty()) {
        variants.push_back(VariantSpec{});
    }

    std::vector<std::filesystem::path> includeDirs;
    includeDirs.push_back(inputPath.parent_path());
    if (source.contains("includeDirs") && source["includeDirs"].is_array()) {
        for (const auto &entry : source["includeDirs"]) {
            if (!entry.is_string()) {
                continue;
            }
            includeDirs.push_back((inputPath.parent_path() / entry.get<std::string>()).lexically_normal());
        }
    }

    std::filesystem::create_directories(outputDir);

    const auto tempDir = (outputDir / ".jzshader_tmp" / shaderName).lexically_normal();
    std::filesystem::create_directories(tempDir);

    std::vector<BlobChunk> chunks;
    uint32_t               nextChunkId = 1;

    Json runtimeManifest;
    runtimeManifest["version"]           = 1;
    runtimeManifest["shaderName"]        = shaderName;
    runtimeManifest["sourceHash"]        = source.value("sourceHash", "");
    runtimeManifest["blob"]              = shaderName + ".jzsblob";
    runtimeManifest["keywords"]          = Json::array();
    runtimeManifest["targets"]           = Json::array({"OpenGL", "Vulkan", "D3D12", "Metal"});
    runtimeManifest["reflectionLayouts"] = Json::object();
    runtimeManifest["vertexLayouts"]     = source.value("vertexLayouts", Json::object());
    runtimeManifest["variants"]          = Json::array();

    for (const auto &keyword : keywords) {
        runtimeManifest["keywords"].push_back({
            {"name", keyword.name},
            {"bit", keyword.bit},
        });
    }

    std::string hashInput;

    const auto workspaceRoot = inputPath.parent_path();

    for (const auto &variant : variants) {
        const auto defines = BuildDefines(variant, keywords);

        Json variantJson;
        variantJson["keywordMask"]  = variant.keywordMask;
        variantJson["vertexLayout"] = variant.vertexLayout;
        variantJson["renderState"]  = variant.renderState.is_null() ? source.value("renderState", Json::object()) : variant.renderState;
        variantJson["targets"]      = Json::array();

        Json openGlTarget = {{"rhi", "OpenGL"}, {"stages", Json::array()}};
        Json vulkanTarget = {{"rhi", "Vulkan"}, {"stages", Json::array()}};
        Json d3d12Target  = {{"rhi", "D3D12"}, {"stages", Json::array()}};
        Json metalTarget  = {{"rhi", "Metal"}, {"stages", Json::array()}};

        for (const auto &[stageName, stage] : stageSpecs) {
            StageArtifacts artifacts;
            if (!CompileStage(stage, defines, includeDirs, workspaceRoot, tempDir, artifacts)) {
                std::cerr << "Failed to compile stage: " << stageName << std::endl;
                return 1;
            }

            const std::string reflectionKey = stageName + "_Mask" + std::to_string(variant.keywordMask);
            if (runtimeManifest["reflectionLayouts"].contains(reflectionKey)) {
                if (runtimeManifest["reflectionLayouts"][reflectionKey] != artifacts.reflectionLayout) {
                    std::cerr << "Reflection layout mismatch for key: " << reflectionKey << std::endl;
                    return 1;
                }
            } else {
                runtimeManifest["reflectionLayouts"][reflectionKey] = artifacts.reflectionLayout;
            }

            hashInput += artifacts.glsl;

            BlobChunk glslChunk{nextChunkId++, std::vector<uint8_t>(artifacts.glsl.begin(), artifacts.glsl.end())};
            BlobChunk spirvChunk{nextChunkId++, artifacts.spirv};
            BlobChunk dxilChunk{nextChunkId++, artifacts.dxil};
            BlobChunk mslChunk{nextChunkId++, std::vector<uint8_t>(artifacts.msl.begin(), artifacts.msl.end())};

            const uint32_t glslChunkId  = glslChunk.id;
            const uint32_t spirvChunkId = spirvChunk.id;
            const uint32_t dxilChunkId  = dxilChunk.id;
            const uint32_t mslChunkId   = mslChunk.id;

            chunks.push_back(std::move(glslChunk));
            chunks.push_back(std::move(spirvChunk));
            chunks.push_back(std::move(dxilChunk));
            chunks.push_back(std::move(mslChunk));

            openGlTarget["stages"].push_back({
                {"stage", stageName},
                {"format", "GLSL"},
                {"chunk", glslChunkId},
                {"entryPoint", stage.entryPoint},
                {"reflectionKey", reflectionKey},
            });

            vulkanTarget["stages"].push_back({
                {"stage", stageName},
                {"format", "SPIRV"},
                {"chunk", spirvChunkId},
                {"entryPoint", stage.entryPoint},
                {"reflectionKey", reflectionKey},
            });

            d3d12Target["stages"].push_back({
                {"stage", stageName},
                {"format", "DXIL"},
                {"chunk", dxilChunkId},
                {"entryPoint", stage.entryPoint},
                {"reflectionKey", reflectionKey},
            });

            metalTarget["stages"].push_back({
                {"stage", stageName},
                {"format", "MSL"},
                {"chunk", mslChunkId},
                {"entryPoint", stage.entryPoint},
                {"reflectionKey", reflectionKey},
            });
        }

        variantJson["targets"].push_back(std::move(openGlTarget));
        variantJson["targets"].push_back(std::move(vulkanTarget));
        variantJson["targets"].push_back(std::move(d3d12Target));
        variantJson["targets"].push_back(std::move(metalTarget));

        runtimeManifest["variants"].push_back(std::move(variantJson));
    }

    if (runtimeManifest["sourceHash"].get<std::string>().empty()) {
        runtimeManifest["sourceHash"] = Fnv1a64Hex(hashInput);
    }

    const auto blobPath = (outputDir / (shaderName + ".jzsblob")).lexically_normal();
    if (!WriteBlob(blobPath, chunks)) {
        std::cerr << "Failed to write shader blob: " << blobPath << std::endl;
        return 1;
    }

    const auto    manifestPath = (outputDir / (shaderName + ".jzshader")).lexically_normal();
    std::ofstream manifestOut(manifestPath, std::ios::trunc);
    if (!manifestOut.is_open()) {
        std::cerr << "Failed to write cooked manifest: " << manifestPath << std::endl;
        return 1;
    }

    manifestOut << runtimeManifest.dump(2);
    std::cout << "Cooked shader: " << manifestPath << std::endl;
    std::cout << "Cooked blob:   " << blobPath << std::endl;

    return 0;
}

void PrintUsage(const char *argv0)
{
    std::cout << "Usage: " << argv0 << " --input <shader.jzshader.src.json> --output-dir <dir>\n";
}

} // namespace

int main(int argc, char **argv)
{
    std::filesystem::path inputPath;
    std::filesystem::path outputDir;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--input" && i + 1 < argc) {
            inputPath = argv[++i];
        } else if (arg == "--output-dir" && i + 1 < argc) {
            outputDir = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            PrintUsage(argv[0]);
            return 0;
        }
    }

    if (inputPath.empty() || outputDir.empty()) {
        PrintUsage(argv[0]);
        return 1;
    }

    try {
        return RunCook(inputPath, outputDir);
    } catch (const std::exception &e) {
        std::cerr << "JzREShaderTool failed: " << e.what() << std::endl;
        return 1;
    }
}
