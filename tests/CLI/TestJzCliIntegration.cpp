/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "JzRE/CLI/JzCliContext.h"
#include "JzRE/CLI/commands/JzAssetCommand.h"
#include "JzRE/CLI/commands/JzProjectCommand.h"
#include "JzRE/CLI/commands/JzSceneCommand.h"
#include "JzRE/CLI/commands/JzShaderCommand.h"

namespace {

using Json = nlohmann::json;

std::filesystem::path MakeUniqueTempDir(const char *prefix)
{
    const auto base = std::filesystem::temp_directory_path();
    const auto now  = std::chrono::steady_clock::now().time_since_epoch().count();

    for (int i = 0; i < 64; ++i) {
        const auto candidate = base / (std::string(prefix) + "_" + std::to_string(now) + "_" + std::to_string(i));
        std::error_code ec;
        if (std::filesystem::create_directories(candidate, ec)) {
            return candidate;
        }
    }

    throw std::runtime_error("failed to create temp directory");
}

void WriteTextFile(const std::filesystem::path &filePath, const std::string &text)
{
    std::filesystem::create_directories(filePath.parent_path());
    std::ofstream file(filePath, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        throw std::runtime_error("failed to open output file");
    }
    file << text;
}

std::filesystem::path FindProjectFile(const std::filesystem::path &projectDir)
{
    for (const auto &entry : std::filesystem::directory_iterator(projectDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".jzreproject") {
            return entry.path();
        }
    }
    throw std::runtime_error("project file not found");
}

JzRE::JzCliResult CreateProject(JzRE::JzProjectCommand             &command,
                                JzRE::JzCliContext                 &context,
                                const std::filesystem::path        &projectDir,
                                const JzRE::String                 &projectName,
                                JzRE::JzCliOutputFormat             format = JzRE::JzCliOutputFormat::Text)
{
    const std::vector<JzRE::String> args = {
        "create",
        "--name", projectName,
        "--dir", projectDir.string(),
    };
    return command.Execute(context, args, format);
}

} // namespace

TEST(JzCliIntegration, ProjectCreateValidateInfoSet)
{
    const auto tempRoot   = MakeUniqueTempDir("JzRE_cli_project");
    const auto projectDir = tempRoot / "Project";

    JzRE::JzCliContext context;
    ASSERT_TRUE(context.Initialize());

    JzRE::JzProjectCommand project;

    const auto createResult = CreateProject(project, context, projectDir, "DemoProject");
    ASSERT_TRUE(createResult.IsSuccess()) << createResult.message;

    const auto projectFile = FindProjectFile(projectDir);

    const std::vector<JzRE::String> validateArgs = {
        "validate",
        "--project", projectFile.string(),
    };
    const auto validateResult = project.Execute(context, validateArgs, JzRE::JzCliOutputFormat::Text);
    ASSERT_TRUE(validateResult.IsSuccess()) << validateResult.message;

    const std::vector<JzRE::String> infoArgs = {
        "info",
        "--project", projectFile.string(),
    };
    const auto infoResult = project.Execute(context, infoArgs, JzRE::JzCliOutputFormat::Json);
    ASSERT_TRUE(infoResult.IsSuccess()) << infoResult.message;

    auto infoPayload = Json::parse(infoResult.message);
    EXPECT_EQ(infoPayload.at("project_name").get<std::string>(), "DemoProject");

    const std::vector<JzRE::String> setArgs = {
        "set",
        "--project", projectFile.string(),
        "--default-scene", "Content/Scenes/Main.jzscene",
        "--render-api", "opengl",
        "--shader-auto-cook", "off",
        "--startup-mode", "host",
    };
    const auto setResult = project.Execute(context, setArgs, JzRE::JzCliOutputFormat::Text);
    ASSERT_TRUE(setResult.IsSuccess()) << setResult.message;

    const auto infoAfterSet = project.Execute(context, infoArgs, JzRE::JzCliOutputFormat::Json);
    ASSERT_TRUE(infoAfterSet.IsSuccess()) << infoAfterSet.message;

    auto setPayload = Json::parse(infoAfterSet.message);
    EXPECT_EQ(setPayload.at("default_scene").get<std::string>(), "Content/Scenes/Main.jzscene");
    EXPECT_EQ(setPayload.at("render_api").get<std::string>(), "opengl");
    EXPECT_EQ(setPayload.at("startup_mode").get<std::string>(), "host");
    EXPECT_FALSE(setPayload.at("shader_auto_cook").get<bool>());

    context.Shutdown();
    std::error_code ec;
    std::filesystem::remove_all(tempRoot, ec);
}

TEST(JzCliIntegration, AssetImportExport)
{
    const auto tempRoot   = MakeUniqueTempDir("JzRE_cli_asset");
    const auto projectDir = tempRoot / "Project";
    const auto outDir     = tempRoot / "Exported";

    JzRE::JzCliContext context;
    ASSERT_TRUE(context.Initialize());

    JzRE::JzProjectCommand project;
    const auto createResult = CreateProject(project, context, projectDir, "AssetProject");
    ASSERT_TRUE(createResult.IsSuccess()) << createResult.message;

    const auto projectFile = FindProjectFile(projectDir);

    const auto sourceFile = tempRoot / "source" / "tex.png";
    WriteTextFile(sourceFile, "dummy-texture-data");

    JzRE::JzAssetCommand asset;

    const std::vector<JzRE::String> importArgs = {
        "import",
        "--project", projectFile.string(),
        "--src", sourceFile.string(),
        "--subfolder", "Textures",
    };
    const auto importResult = asset.Execute(context, importArgs, JzRE::JzCliOutputFormat::Text);
    ASSERT_TRUE(importResult.IsSuccess()) << importResult.message;

    const auto importedFile = projectDir / "Content" / "Textures" / "tex.png";
    ASSERT_TRUE(std::filesystem::exists(importedFile));

    const std::vector<JzRE::String> exportArgs = {
        "export",
        "--project", projectFile.string(),
        "--src", "Textures/tex.png",
        "--out", outDir.string(),
    };
    const auto exportResult = asset.Execute(context, exportArgs, JzRE::JzCliOutputFormat::Text);
    ASSERT_TRUE(exportResult.IsSuccess()) << exportResult.message;

    const auto exportedFile = outDir / "Textures" / "tex.png";
    ASSERT_TRUE(std::filesystem::exists(exportedFile));

    context.Shutdown();
    std::error_code ec;
    std::filesystem::remove_all(tempRoot, ec);
}

TEST(JzCliIntegration, ShaderCookAndCookProjectWithMockTool)
{
    const auto tempRoot   = MakeUniqueTempDir("JzRE_cli_shader");
    const auto projectDir = tempRoot / "Project";

    const auto toolPath = tempRoot / "mock-shader-tool.sh";
    WriteTextFile(toolPath, "#!/bin/sh\nexit 0\n");
    std::error_code permEc;
    std::filesystem::permissions(
        toolPath,
        std::filesystem::perms::owner_exec |
        std::filesystem::perms::group_exec |
        std::filesystem::perms::others_exec,
        std::filesystem::perm_options::add,
        permEc);
    ASSERT_FALSE(permEc);

    const auto inputDir = tempRoot / "shader-src";
    const auto outDir   = tempRoot / "shader-out";
    WriteTextFile(inputDir / "unit.jzshader.src.json", "{}\n");

    JzRE::JzCliContext context;
    ASSERT_TRUE(context.Initialize());

    JzRE::JzShaderCommand shader;
    {
        const std::vector<JzRE::String> cookArgs = {
            "cook",
            "--input", inputDir.string(),
            "--output-dir", outDir.string(),
            "--tool", toolPath.string(),
        };
        const auto cookResult = shader.Execute(context, cookArgs, JzRE::JzCliOutputFormat::Json);
        ASSERT_TRUE(cookResult.IsSuccess()) << cookResult.message;

        const auto payload = Json::parse(cookResult.message);
        EXPECT_EQ(payload.at("cooked").get<int>(), 1);
        EXPECT_EQ(payload.at("total").get<int>(), 1);
    }

    JzRE::JzProjectCommand project;
    const auto createResult = CreateProject(project, context, projectDir, "ShaderProject");
    ASSERT_TRUE(createResult.IsSuccess()) << createResult.message;

    const auto projectFile = FindProjectFile(projectDir);
    WriteTextFile(projectDir / "Content" / "Shaders" / "src" / "project.jzshader.src.json", "{}\n");

    {
        const std::vector<JzRE::String> cookProjectArgs = {
            "cook-project",
            "--project", projectFile.string(),
            "--tool", toolPath.string(),
        };
        const auto cookProjectResult = shader.Execute(context, cookProjectArgs, JzRE::JzCliOutputFormat::Json);
        ASSERT_TRUE(cookProjectResult.IsSuccess()) << cookProjectResult.message;

        const auto payload = Json::parse(cookProjectResult.message);
        EXPECT_EQ(payload.at("cooked").get<int>(), 1);
        EXPECT_EQ(payload.at("total").get<int>(), 1);
    }

    context.Shutdown();
    std::error_code ec;
    std::filesystem::remove_all(tempRoot, ec);
}

TEST(JzCliIntegration, SceneValidateAndStats)
{
    const auto tempRoot = MakeUniqueTempDir("JzRE_cli_scene");
    const auto sceneFile = tempRoot / "main.jzscene";

    WriteTextFile(sceneFile,
                  "{\n"
                  "  \"version\": 1,\n"
                  "  \"entities\": [\n"
                  "    {\"name\": \"Camera\", \"transform\": {}},\n"
                  "    {\"name\": \"Cube\", \"tags\": [\"demo\"]}\n"
                  "  ]\n"
                  "}\n");

    JzRE::JzCliContext context;
    ASSERT_TRUE(context.Initialize());

    JzRE::JzSceneCommand scene;

    {
        const std::vector<JzRE::String> validateArgs = {
            "validate",
            "--file", sceneFile.string(),
        };
        const auto validateResult = scene.Execute(context, validateArgs, JzRE::JzCliOutputFormat::Text);
        ASSERT_TRUE(validateResult.IsSuccess()) << validateResult.message;
    }

    {
        const std::vector<JzRE::String> statsArgs = {
            "stats",
            "--file", sceneFile.string(),
        };
        const auto statsResult = scene.Execute(context, statsArgs, JzRE::JzCliOutputFormat::Json);
        ASSERT_TRUE(statsResult.IsSuccess()) << statsResult.message;

        const auto payload = Json::parse(statsResult.message);
        EXPECT_EQ(payload.at("entities").get<int>(), 2);
        EXPECT_EQ(payload.at("named_entities").get<int>(), 2);
        EXPECT_EQ(payload.at("with_transform").get<int>(), 1);
    }

    context.Shutdown();
    std::error_code ec;
    std::filesystem::remove_all(tempRoot, ec);
}
