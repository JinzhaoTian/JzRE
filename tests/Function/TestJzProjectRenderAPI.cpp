/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include <filesystem>
#include <random>

#include <gtest/gtest.h>

#include "JzRE/Runtime/Function/Project/JzProjectManager.h"

namespace {

std::filesystem::path MakeTempProjectPath(const char *suffix)
{
    std::mt19937_64 rng{std::random_device{}()};
    const auto      id = static_cast<unsigned long long>(rng());
    return std::filesystem::temp_directory_path() / (std::string("jzre_render_api_") + suffix + "_" + std::to_string(id));
}

void CleanupPath(const std::filesystem::path &path)
{
    std::error_code ec;
    std::filesystem::remove_all(path, ec);
}

} // namespace

TEST(JzProjectRenderAPI, PersistsD3D12RenderAPI)
{
    const auto projectPath = MakeTempProjectPath("d3d12");
    CleanupPath(projectPath);

    JzRE::JzProjectManager manager;
    ASSERT_EQ(manager.CreateProject(projectPath, "RenderApiD3D12"), JzRE::JzEProjectResult::Success);

    manager.GetConfig().renderAPI = JzRE::JzERenderAPI::D3D12;
    ASSERT_EQ(manager.SaveProject(), JzRE::JzEProjectResult::Success);

    JzRE::JzProjectManager reloaded;
    ASSERT_EQ(reloaded.LoadProject(manager.GetProjectFilePath()), JzRE::JzEProjectResult::Success);
    EXPECT_EQ(reloaded.GetConfig().renderAPI, JzRE::JzERenderAPI::D3D12);

    CleanupPath(projectPath);
}

TEST(JzProjectRenderAPI, PersistsMetalRenderAPI)
{
    const auto projectPath = MakeTempProjectPath("metal");
    CleanupPath(projectPath);

    JzRE::JzProjectManager manager;
    ASSERT_EQ(manager.CreateProject(projectPath, "RenderApiMetal"), JzRE::JzEProjectResult::Success);

    manager.GetConfig().renderAPI = JzRE::JzERenderAPI::Metal;
    ASSERT_EQ(manager.SaveProject(), JzRE::JzEProjectResult::Success);

    JzRE::JzProjectManager reloaded;
    ASSERT_EQ(reloaded.LoadProject(manager.GetProjectFilePath()), JzRE::JzEProjectResult::Success);
    EXPECT_EQ(reloaded.GetConfig().renderAPI, JzRE::JzERenderAPI::Metal);

    CleanupPath(projectPath);
}
