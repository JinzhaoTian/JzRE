/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/JzContext.h"
#include "JzRE/Runtime/Resource/JzResourceManager.h"

// Resource types
#include "JzRE/Runtime/Resource/JzTexture.h"
#include "JzRE/Runtime/Resource/JzMesh.h"
#include "JzRE/Runtime/Resource/JzModel.h"
#include "JzRE/Runtime/Resource/JzMaterial.h"
#include "JzRE/Runtime/Resource/JzShader.h"
#include "JzRE/Runtime/Resource/JzFont.h"

// Resource factories
#include "JzRE/Runtime/Resource/JzTextureFactory.h"
#include "JzRE/Runtime/Resource/JzMeshFactory.h"
#include "JzRE/Runtime/Resource/JzModelFactory.h"
#include "JzRE/Runtime/Resource/JzMaterialFactory.h"
#include "JzRE/Runtime/Resource/JzShaderFactory.h"
#include "JzRE/Runtime/Resource/JzFontFactory.h"

JzRE::JzContext &JzRE::JzContext::GetInstance()
{
    static JzContext instance;
    return instance;
}

JzRE::Bool JzRE::JzContext::InitializeEngine(JzResourceManager &resourceManager)
{
    // Already initialized - return success
    if (m_state == JzEContextState::EngineInitialized || m_state == JzEContextState::ProjectInitialized) {
        return true;
    }

    // Store engine working directory
    m_enginePath = std::filesystem::current_path();

    // Register all resource factories
    RegisterAllFactories(resourceManager);

    // Setup engine search paths
    SetupEngineSearchPaths(resourceManager);

    m_state = JzEContextState::EngineInitialized;
    return true;
}

JzRE::Bool JzRE::JzContext::InitializeProject(JzResourceManager           &resourceManager,
                                              const std::filesystem::path &projectPath)
{
    // Engine must be initialized first
    if (m_state == JzEContextState::Uninitialized) {
        return false;
    }

    // Validate project path
    if (!std::filesystem::exists(projectPath)) {
        m_state = JzEContextState::Error;
        return false;
    }

    // Store project path
    m_projectPath = projectPath;

    // Setup project-specific search paths
    SetupProjectSearchPaths(resourceManager, projectPath);

    m_state = JzEContextState::ProjectInitialized;
    return true;
}

JzRE::Bool JzRE::JzContext::IsEngineInitialized() const
{
    return m_state == JzEContextState::EngineInitialized || m_state == JzEContextState::ProjectInitialized;
}

JzRE::Bool JzRE::JzContext::IsProjectInitialized() const
{
    return m_state == JzEContextState::ProjectInitialized;
}

JzRE::JzEContextState JzRE::JzContext::GetState() const
{
    return m_state;
}

std::filesystem::path JzRE::JzContext::GetEnginePath() const
{
    return m_enginePath;
}

std::filesystem::path JzRE::JzContext::GetProjectPath() const
{
    return m_projectPath;
}

void JzRE::JzContext::Shutdown()
{
    m_state = JzEContextState::Uninitialized;
    m_enginePath.clear();
    m_projectPath.clear();
}

void JzRE::JzContext::RegisterAllFactories(JzResourceManager &resourceManager)
{
    // Register all resource type factories
    resourceManager.RegisterFactory<JzTexture>(std::make_unique<JzTextureFactory>());
    resourceManager.RegisterFactory<JzMesh>(std::make_unique<JzMeshFactory>());
    resourceManager.RegisterFactory<JzModel>(std::make_unique<JzModelFactory>());
    resourceManager.RegisterFactory<JzMaterial>(std::make_unique<JzMaterialFactory>());
    resourceManager.RegisterFactory<JzShader>(std::make_unique<JzShaderFactory>());
    resourceManager.RegisterFactory<JzFont>(std::make_unique<JzFontFactory>());
}

void JzRE::JzContext::SetupEngineSearchPaths(JzResourceManager &resourceManager)
{
    // Engine-level search paths for built-in resources
    resourceManager.AddSearchPath("./icons");
    resourceManager.AddSearchPath("./shaders");
}

void JzRE::JzContext::SetupProjectSearchPaths(JzResourceManager           &resourceManager,
                                              const std::filesystem::path &projectPath)
{
    // Convention-based project resource directories
    const std::vector<std::string> conventionPaths = {
        "assets",
        "textures",
        "models",
        "shaders",
        "materials",
        "fonts"};

    // Add project root
    resourceManager.AddSearchPath(projectPath.string());

    // Add convention-based subdirectories if they exist
    for (const auto &subdir : conventionPaths) {
        auto fullPath = projectPath / subdir;
        if (std::filesystem::exists(fullPath) && std::filesystem::is_directory(fullPath)) {
            resourceManager.AddSearchPath(fullPath.string());
        }
    }
}
