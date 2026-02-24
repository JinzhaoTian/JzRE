/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include <filesystem>
#include <fstream>
#include <random>
#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Resource/JzShader.h"

namespace {

using Json = nlohmann::json;

struct BlobHeader {
    char     magic[4]   = {'J', 'Z', 'S', 'B'};
    uint32_t version    = 1;
    uint32_t chunkCount = 0;
    uint32_t reserved   = 0;
};

struct BlobChunkHeader {
    uint32_t id     = 0;
    uint32_t offset = 0;
    uint32_t size   = 0;
    uint32_t flags  = 0;
};

class JzTestPipeline final : public JzRE::JzRHIPipeline {
public:
    explicit JzTestPipeline(const JzRE::JzPipelineDesc &desc) :
        JzRHIPipeline(desc)
    { }

    void CommitParameters() override
    {
        MarkParametersCommitted();
    }
};

class JzTestDevice final : public JzRE::JzDevice {
public:
    explicit JzTestDevice(JzRE::JzERHIType type) :
        JzDevice(type)
    { }

    JzRE::String GetDeviceName() const override
    {
        return "TestDevice";
    }

    JzRE::String GetVendorName() const override
    {
        return "JzRE";
    }

    JzRE::String GetDriverVersion() const override
    {
        return "1.0";
    }

    std::shared_ptr<JzRE::JzGPUBufferObject> CreateBuffer(const JzRE::JzGPUBufferObjectDesc &) override
    {
        return nullptr;
    }

    std::shared_ptr<JzRE::JzGPUTextureObject> CreateTexture(const JzRE::JzGPUTextureObjectDesc &) override
    {
        return nullptr;
    }

    std::shared_ptr<JzRE::JzGPUShaderProgramObject> CreateShader(const JzRE::JzShaderProgramDesc &) override
    {
        return nullptr;
    }

    std::shared_ptr<JzRE::JzRHIPipeline> CreatePipeline(const JzRE::JzPipelineDesc &desc) override
    {
        m_lastPipelineDesc = desc;
        return std::make_shared<JzTestPipeline>(desc);
    }

    std::shared_ptr<JzRE::JzGPUFramebufferObject> CreateFramebuffer(const JzRE::String &) override
    {
        return nullptr;
    }

    std::shared_ptr<JzRE::JzGPUVertexArrayObject> CreateVertexArray(const JzRE::String &) override
    {
        return nullptr;
    }

    std::shared_ptr<JzRE::JzRHICommandList> CreateCommandList(const JzRE::String &) override
    {
        return nullptr;
    }

    void       ExecuteCommandList(std::shared_ptr<JzRE::JzRHICommandList>) override { }
    void       ExecuteCommandLists(const std::vector<std::shared_ptr<JzRE::JzRHICommandList>> &) override { }
    void       BeginFrame() override { }
    void       EndFrame() override { }
    void       Flush() override { }
    void       Finish() override { }
    JzRE::Bool SupportsMultithreading() const override
    {
        return false;
    }

    const JzRE::JzPipelineDesc &GetLastPipelineDesc() const
    {
        return m_lastPipelineDesc;
    }

private:
    JzRE::JzPipelineDesc m_lastPipelineDesc{};
};

std::filesystem::path MakeTempDirectory(const char *suffix)
{
    std::mt19937_64 rng{std::random_device{}()};
    const auto      id = static_cast<unsigned long long>(rng());
    return std::filesystem::temp_directory_path() / (std::string("jzre_shader_") + suffix + "_" + std::to_string(id));
}

void CleanupPath(const std::filesystem::path &path)
{
    std::error_code ec;
    std::filesystem::remove_all(path, ec);
}

bool WriteTextFile(const std::filesystem::path &path, const std::string &text)
{
    std::ofstream file(path, std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }
    file << text;
    return file.good();
}

bool WriteBlobFile(const std::filesystem::path &path, const std::vector<std::pair<uint32_t, std::vector<uint8_t>>> &chunks)
{
    BlobHeader header{};
    header.chunkCount = static_cast<uint32_t>(chunks.size());

    std::vector<BlobChunkHeader> table;
    table.reserve(chunks.size());

    uint32_t offset = static_cast<uint32_t>(sizeof(BlobHeader) + chunks.size() * sizeof(BlobChunkHeader));
    for (const auto &[chunkId, data] : chunks) {
        BlobChunkHeader chunkHeader{};
        chunkHeader.id     = chunkId;
        chunkHeader.offset = offset;
        chunkHeader.size   = static_cast<uint32_t>(data.size());
        table.push_back(chunkHeader);
        offset += chunkHeader.size;
    }

    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out.is_open()) {
        return false;
    }

    out.write(reinterpret_cast<const char *>(&header), sizeof(header));
    out.write(reinterpret_cast<const char *>(table.data()),
              static_cast<std::streamsize>(table.size() * sizeof(BlobChunkHeader)));

    for (const auto &chunk : chunks) {
        const auto &data = chunk.second;
        if (!data.empty()) {
            out.write(reinterpret_cast<const char *>(data.data()), static_cast<std::streamsize>(data.size()));
        }
    }

    return out.good();
}

std::vector<uint8_t> AsBytes(const std::string &text)
{
    return std::vector<uint8_t>(text.begin(), text.end());
}

Json BuildBaseManifest()
{
    Json manifest;
    manifest["version"]           = 1;
    manifest["shaderName"]        = "unit_shader";
    manifest["sourceHash"]        = "unit_hash";
    manifest["blob"]              = "unit_shader.jzsblob";
    manifest["keywords"]          = Json::array({{{"name", "USE_DIFFUSE_MAP"}, {"bit", 0}}});
    manifest["targets"]           = Json::array({"OpenGL", "Vulkan", "D3D12", "Metal"});
    manifest["reflectionLayouts"] = Json::object({
        {"Vertex_Mask0", {{"resources", Json::array({{{"name", "JzVertexUniforms"}, {"type", "UniformBuffer"}, {"set", 0}, {"binding", 0}, {"arraySize", 1}}})}}},
        {"Fragment_Mask0", {{"resources", Json::array({{{"name", "JzFragmentUniforms"}, {"type", "UniformBuffer"}, {"set", 0}, {"binding", 1}, {"arraySize", 1}}})}}},
        {"Vertex_Mask1", {{"resources", Json::array({{{"name", "JzVertexUniforms"}, {"type", "UniformBuffer"}, {"set", 0}, {"binding", 0}, {"arraySize", 1}}})}}},
        {"Fragment_Mask1", {{"resources", Json::array({{{"name", "JzFragmentUniforms"}, {"type", "UniformBuffer"}, {"set", 0}, {"binding", 1}, {"arraySize", 1}}})}}},
    });
    manifest["vertexLayouts"]     = Json::object({{"default", {{"bindings", Json::array({{{"binding", 0}, {"stride", 20}, {"perInstance", false}}})}, {"attributes", Json::array({{{"location", 0}, {"binding", 0}, {"format", "Float3"}, {"offset", 0}}, {{"location", 1}, {"binding", 0}, {"format", "Float2"}, {"offset", 12}}})}}}});
    manifest["variants"]          = Json::array({{{"keywordMask", 0},
                                                  {"vertexLayout", "default"},
                                                  {"renderState", Json::object()},
                                                  {"targets", Json::array({{{"rhi", "OpenGL"},
                                                                            {"stages", Json::array({{{"stage", "Vertex"}, {"format", "GLSL"}, {"chunk", 1}, {"entryPoint", "VSMain"}, {"reflectionKey", "Vertex_Mask0"}},
                                                                                                    {{"stage", "Fragment"}, {"format", "GLSL"}, {"chunk", 2}, {"entryPoint", "PSMain"}, {"reflectionKey", "Fragment_Mask0"}}})}}})}},
                                                 {{"keywordMask", 1},
                                                  {"vertexLayout", "default"},
                                                  {"renderState", Json::object()},
                                                  {"targets", Json::array({{{"rhi", "OpenGL"},
                                                                            {"stages", Json::array({{{"stage", "Vertex"}, {"format", "GLSL"}, {"chunk", 3}, {"entryPoint", "VSMain"}, {"reflectionKey", "Vertex_Mask1"}},
                                                                                                    {{"stage", "Fragment"}, {"format", "GLSL"}, {"chunk", 4}, {"entryPoint", "PSMain"}, {"reflectionKey", "Fragment_Mask1"}}})}}})}}});
    return manifest;
}

bool WriteCookedShader(const std::filesystem::path &dir, const Json &manifest)
{
    std::filesystem::create_directories(dir);

    const auto manifestPath = dir / "unit_shader.jzshader";
    const auto blobPath     = dir / "unit_shader.jzsblob";

    if (!WriteTextFile(manifestPath, manifest.dump(2))) {
        return false;
    }

    return WriteBlobFile(blobPath, {
                                       {1U, AsBytes("#version 330 core\nvoid main(){}\n")},
                                       {2U, AsBytes("#version 330 core\nout vec4 FragColor;\nvoid main(){FragColor=vec4(1.0);} \n")},
                                       {3U, AsBytes("#version 330 core\nvoid main(){}\n")},
                                       {4U, AsBytes("#version 330 core\nout vec4 FragColor;\nvoid main(){FragColor=vec4(0.5);} \n")},
                                   });
}

} // namespace

TEST(JzShaderCooked, LoadAndResolveVariantsWithShaderLayout)
{
    const auto tempDir = MakeTempDirectory("valid");
    CleanupPath(tempDir);

    auto manifest = BuildBaseManifest();
    ASSERT_TRUE(WriteCookedShader(tempDir, manifest));

    JzRE::JzServiceContainer::Init();
    JzTestDevice testDevice(JzRE::JzERHIType::OpenGL);
    JzRE::JzServiceContainer::Provide<JzRE::JzDevice>(testDevice);

    JzRE::JzShader shader((tempDir / "unit_shader.jzshader").string());
    ASSERT_TRUE(shader.Load());
    EXPECT_TRUE(shader.IsCompiled());

    auto pipeline = shader.GetVariant(1);
    ASSERT_NE(pipeline, nullptr);

    const auto &pipelineDesc = testDevice.GetLastPipelineDesc();
    EXPECT_EQ(pipelineDesc.shaders.size(), 2U);
    EXPECT_EQ(pipelineDesc.shaderLayout.resources.size(), 2U);

    const auto backendDescs = shader.GetBackendProgramDesc(JzRE::JzERHIType::OpenGL, 1);
    EXPECT_EQ(backendDescs.size(), 2U);
    EXPECT_EQ(backendDescs[0].entryPoint, "VSMain");
    EXPECT_EQ(backendDescs[1].entryPoint, "PSMain");

    const JzRE::U64 mask = shader.BuildKeywordMask({{"USE_DIFFUSE_MAP", "1"}});
    EXPECT_EQ(mask, 1ULL);

    CleanupPath(tempDir);
}

TEST(JzShaderCooked, RejectsKeywordBitOverflow)
{
    const auto tempDir = MakeTempDirectory("keyword_overflow");
    CleanupPath(tempDir);

    auto manifest        = BuildBaseManifest();
    manifest["keywords"] = Json::array({{{"name", "TOO_HIGH"}, {"bit", 64}}});
    ASSERT_TRUE(WriteCookedShader(tempDir, manifest));

    JzRE::JzServiceContainer::Init();
    JzTestDevice testDevice(JzRE::JzERHIType::OpenGL);
    JzRE::JzServiceContainer::Provide<JzRE::JzDevice>(testDevice);

    JzRE::JzShader shader((tempDir / "unit_shader.jzshader").string());
    EXPECT_FALSE(shader.Load());
    EXPECT_EQ(shader.GetCompileStatus(), JzRE::JzEShaderCompileStatus::Failed);

    CleanupPath(tempDir);
}

TEST(JzShaderCooked, RejectsMissingEntryPoint)
{
    const auto tempDir = MakeTempDirectory("missing_entry");
    CleanupPath(tempDir);

    auto manifest                                                    = BuildBaseManifest();
    manifest["variants"][0]["targets"][0]["stages"][0]["entryPoint"] = "";
    ASSERT_TRUE(WriteCookedShader(tempDir, manifest));

    JzRE::JzServiceContainer::Init();
    JzTestDevice testDevice(JzRE::JzERHIType::OpenGL);
    JzRE::JzServiceContainer::Provide<JzRE::JzDevice>(testDevice);

    JzRE::JzShader shader((tempDir / "unit_shader.jzshader").string());
    EXPECT_FALSE(shader.Load());
    EXPECT_EQ(shader.GetCompileStatus(), JzRE::JzEShaderCompileStatus::Failed);

    CleanupPath(tempDir);
}

TEST(JzShaderCooked, RejectsReflectionLayoutConflicts)
{
    const auto tempDir = MakeTempDirectory("layout_conflict");
    CleanupPath(tempDir);

    auto manifest                                                = BuildBaseManifest();
    manifest["reflectionLayouts"]["Fragment_Mask0"]["resources"] = Json::array({{{"name", "JzFragmentUniforms"}, {"type", "UniformBuffer"}, {"set", 0}, {"binding", 1}, {"arraySize", 1}},
                                                                                {{"name", "JzFragmentUniforms"}, {"type", "UniformBuffer"}, {"set", 0}, {"binding", 1}, {"arraySize", 2}}});
    ASSERT_TRUE(WriteCookedShader(tempDir, manifest));

    JzRE::JzServiceContainer::Init();
    JzTestDevice testDevice(JzRE::JzERHIType::OpenGL);
    JzRE::JzServiceContainer::Provide<JzRE::JzDevice>(testDevice);

    JzRE::JzShader shader((tempDir / "unit_shader.jzshader").string());
    EXPECT_FALSE(shader.Load());
    EXPECT_EQ(shader.GetCompileStatus(), JzRE::JzEShaderCompileStatus::Failed);

    CleanupPath(tempDir);
}

TEST(JzShaderCooked, RejectsMissingBlobChunk)
{
    const auto tempDir = MakeTempDirectory("missing_chunk");
    CleanupPath(tempDir);

    auto manifest                                               = BuildBaseManifest();
    manifest["variants"][0]["targets"][0]["stages"][0]["chunk"] = 99;
    ASSERT_TRUE(WriteCookedShader(tempDir, manifest));

    JzRE::JzServiceContainer::Init();
    JzTestDevice testDevice(JzRE::JzERHIType::OpenGL);
    JzRE::JzServiceContainer::Provide<JzRE::JzDevice>(testDevice);

    JzRE::JzShader shader((tempDir / "unit_shader.jzshader").string());
    EXPECT_FALSE(shader.Load());
    EXPECT_EQ(shader.GetCompileStatus(), JzRE::JzEShaderCompileStatus::Failed);

    CleanupPath(tempDir);
}

TEST(JzShaderCooked, SupportsStemPathWithoutManifestExtension)
{
    const auto tempDir = MakeTempDirectory("stem_path");
    CleanupPath(tempDir);

    auto manifest = BuildBaseManifest();
    ASSERT_TRUE(WriteCookedShader(tempDir, manifest));

    JzRE::JzServiceContainer::Init();
    JzTestDevice testDevice(JzRE::JzERHIType::OpenGL);
    JzRE::JzServiceContainer::Provide<JzRE::JzDevice>(testDevice);

    JzRE::JzShader shader((tempDir / "unit_shader").string());
    ASSERT_TRUE(shader.Load());
    EXPECT_TRUE(shader.IsCompiled());
    EXPECT_NE(shader.GetMainVariant(), nullptr);

    CleanupPath(tempDir);
}

TEST(JzShaderCooked, RejectsMissingBlobFile)
{
    const auto tempDir = MakeTempDirectory("missing_blob_file");
    CleanupPath(tempDir);

    auto manifest      = BuildBaseManifest();
    manifest["blob"]   = "does_not_exist.jzsblob";
    ASSERT_TRUE(WriteCookedShader(tempDir, manifest));

    JzRE::JzServiceContainer::Init();
    JzTestDevice testDevice(JzRE::JzERHIType::OpenGL);
    JzRE::JzServiceContainer::Provide<JzRE::JzDevice>(testDevice);

    JzRE::JzShader shader((tempDir / "unit_shader.jzshader").string());
    EXPECT_FALSE(shader.Load());
    EXPECT_EQ(shader.GetCompileStatus(), JzRE::JzEShaderCompileStatus::Failed);

    CleanupPath(tempDir);
}
