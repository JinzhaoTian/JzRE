/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
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
#include "JzRE/CLI/commands/JzInitCommand.h"
#include "JzRE/CLI/commands/JzCreateCommand.h"
#include "JzRE/CLI/commands/JzImportCommand.h"
#include "JzRE/CLI/commands/JzBuildCommand.h"

namespace {

using Json = nlohmann::json;

std::filesystem::path MakeUniqueTempDir(const char *prefix)
{
    const auto base = std::filesystem::temp_directory_path();
    const auto now  = std::chrono::steady_clock::now().time_since_epoch().count();

    for (int i = 0; i < 64; ++i) {
        const auto candidate =
            base / (std::string(prefix) + "_" + std::to_string(now) + "_" + std::to_string(i));
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

} // namespace

TEST(JzCliIntegration, InitProject)
{
    const auto tempRoot   = MakeUniqueTempDir("JzRE_cli_init");
    const auto projectDir = tempRoot / "MyGame";

    JzRE::JzCliContext context;
    ASSERT_TRUE(context.Initialize());

    JzRE::JzInitCommand init;

    const std::vector<JzRE::String> args = {
        projectDir.string(),
        "--name",
        "MyGame",
        "--render-api",
        "opengl",
    };
    const auto result = init.Execute(context, args, JzRE::JzCliOutputFormat::Json);
    ASSERT_TRUE(result.IsSuccess()) << result.message;

    const auto payload = Json::parse(result.message);
    EXPECT_EQ(payload.at("project_name").get<std::string>(), "MyGame");
    EXPECT_EQ(payload.at("render_api").get<std::string>(), "opengl");
    EXPECT_TRUE(std::filesystem::exists(payload.at("project").get<std::string>()));

    context.Shutdown();
    std::error_code ec;
    std::filesystem::remove_all(tempRoot, ec);
}

TEST(JzCliIntegration, InitProjectDefaultName)
{
    const auto tempRoot   = MakeUniqueTempDir("JzRE_cli_init_name");
    const auto projectDir = tempRoot / "NamedByDir";

    JzRE::JzCliContext context;
    ASSERT_TRUE(context.Initialize());

    JzRE::JzInitCommand init;

    // No --name: should default to directory name "NamedByDir"
    const std::vector<JzRE::String> args   = {projectDir.string()};
    const auto                      result = init.Execute(context, args, JzRE::JzCliOutputFormat::Json);
    ASSERT_TRUE(result.IsSuccess()) << result.message;

    const auto payload = Json::parse(result.message);
    EXPECT_EQ(payload.at("project_name").get<std::string>(), "NamedByDir");

    context.Shutdown();
    std::error_code ec;
    std::filesystem::remove_all(tempRoot, ec);
}

TEST(JzCliIntegration, ImportAsset)
{
    const auto tempRoot   = MakeUniqueTempDir("JzRE_cli_import");
    const auto projectDir = tempRoot / "Project";

    JzRE::JzCliContext context;
    ASSERT_TRUE(context.Initialize());

    // Init the project first
    JzRE::JzInitCommand             init;
    const std::vector<JzRE::String> initArgs = {projectDir.string(), "--name", "AssetProject"};
    ASSERT_TRUE(init.Execute(context, initArgs, JzRE::JzCliOutputFormat::Text).IsSuccess());

    const auto projectFile = FindProjectFile(projectDir);

    // Write a dummy asset to import
    const auto sourceFile = tempRoot / "source" / "tex.png";
    WriteTextFile(sourceFile, "dummy-texture-data");

    JzRE::JzImportCommand import_cmd;

    const std::vector<JzRE::String> importArgs = {
        sourceFile.string(),
        "--project",
        projectFile.string(),
        "--subfolder",
        "Textures",
    };
    const auto importResult =
        import_cmd.Execute(context, importArgs, JzRE::JzCliOutputFormat::Text);
    ASSERT_TRUE(importResult.IsSuccess()) << importResult.message;

    const auto importedFile = projectDir / "Content" / "Textures" / "tex.png";
    EXPECT_TRUE(std::filesystem::exists(importedFile));

    context.Shutdown();
    std::error_code ec;
    std::filesystem::remove_all(tempRoot, ec);
}

TEST(JzCliIntegration, BuildProjectNoShaders)
{
    const auto tempRoot   = MakeUniqueTempDir("JzRE_cli_build");
    const auto projectDir = tempRoot / "Project";

    JzRE::JzCliContext context;
    ASSERT_TRUE(context.Initialize());

    // Init a project (no shader sources → build should succeed trivially)
    JzRE::JzInitCommand             init;
    const std::vector<JzRE::String> initArgs = {projectDir.string(), "--name", "BuildProject"};
    ASSERT_TRUE(init.Execute(context, initArgs, JzRE::JzCliOutputFormat::Text).IsSuccess());

    const auto projectFile = FindProjectFile(projectDir);

    JzRE::JzBuildCommand build;

    const std::vector<JzRE::String> buildArgs = {
        "--project",
        projectFile.string(),
    };
    const auto buildResult = build.Execute(context, buildArgs, JzRE::JzCliOutputFormat::Json);
    ASSERT_TRUE(buildResult.IsSuccess()) << buildResult.message;

    const auto payload = Json::parse(buildResult.message);
    EXPECT_TRUE(payload.at("ok").get<bool>());

    // Stamp file should have been written
    const auto stampPath = projectDir / ".jzre-built";
    EXPECT_TRUE(std::filesystem::exists(stampPath));

    context.Shutdown();
    std::error_code ec;
    std::filesystem::remove_all(tempRoot, ec);
}

TEST(JzCliIntegration, CreateShaderTemplate)
{
    const auto tempRoot = MakeUniqueTempDir("JzRE_cli_create");

    JzRE::JzCliContext context;
    ASSERT_TRUE(context.Initialize());

    JzRE::JzCreateCommand create;

    const std::vector<JzRE::String> args = {
        "shader",
        "MyShader",
        "--dir",
        tempRoot.string(),
    };
    const auto result = create.Execute(context, args, JzRE::JzCliOutputFormat::Json);
    ASSERT_TRUE(result.IsSuccess()) << result.message;

    const auto payload  = Json::parse(result.message);
    const auto filePath = std::filesystem::path(payload.at("path").get<std::string>());
    EXPECT_TRUE(std::filesystem::exists(filePath));
    EXPECT_EQ(filePath.filename().string(), "MyShader.jzshader.src.json");

    // Verify content is valid JSON with expected keys
    std::ifstream ifs(filePath);
    const auto    content = Json::parse(ifs);
    EXPECT_EQ(content.at("name").get<std::string>(), "MyShader");

    context.Shutdown();
    std::error_code ec;
    std::filesystem::remove_all(tempRoot, ec);
}

TEST(JzCliIntegration, CreateScriptTemplate)
{
    const auto tempRoot = MakeUniqueTempDir("JzRE_cli_create_script");

    JzRE::JzCliContext context;
    ASSERT_TRUE(context.Initialize());

    JzRE::JzCreateCommand create;

    const std::vector<JzRE::String> args = {
        "script",
        "Player",
        "--dir",
        tempRoot.string(),
    };
    const auto result = create.Execute(context, args, JzRE::JzCliOutputFormat::Json);
    ASSERT_TRUE(result.IsSuccess()) << result.message;

    const auto payload  = Json::parse(result.message);
    const auto filePath = std::filesystem::path(payload.at("path").get<std::string>());
    EXPECT_TRUE(std::filesystem::exists(filePath));
    EXPECT_EQ(filePath.filename().string(), "Player.lua");

    context.Shutdown();
    std::error_code ec;
    std::filesystem::remove_all(tempRoot, ec);
}
