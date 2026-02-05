/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzREInstance.h"

namespace {

JzRE::JzRERuntimeSettings CreateSettingsFromPath(JzRE::JzERHIType rhiType,
                                                  const std::filesystem::path &openPath)
{
    JzRE::JzRERuntimeSettings settings;
    settings.windowTitle = "JzRE";
    settings.windowSize  = {1280, 720};
    settings.rhiType     = rhiType;

    // Check if path is a project file or directory
    if (!openPath.empty()) {
        auto projectFile = JzRE::JzREInstance::FindProjectFile(openPath);
        if (!projectFile.empty()) {
            settings.projectFile = projectFile;
        }
    }

    return settings;
}

} // anonymous namespace

std::filesystem::path JzRE::JzREInstance::FindProjectFile(const std::filesystem::path &path)
{
    if (path.empty()) {
        return {};
    }

    // If path is a file with .jzreproject extension, use it directly
    if (std::filesystem::is_regular_file(path)) {
        if (path.extension() == JzProjectManager::GetProjectFileExtension()) {
            return path;
        }
        return {};
    }

    // If path is a directory, search for .jzreproject file
    if (std::filesystem::is_directory(path)) {
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file() &&
                entry.path().extension() == JzProjectManager::GetProjectFileExtension()) {
                return entry.path();
            }
        }
    }

    return {};
}

JzRE::JzREInstance::JzREInstance(JzERHIType rhiType, const std::filesystem::path &openPath) :
    JzRERuntime(CreateSettingsFromPath(rhiType, openPath)),
    m_openPath(openPath)
{
    // Create editor with runtime reference
    m_editor = std::make_unique<JzEditor>(*this);
}

JzRE::JzREInstance::~JzREInstance()
{
    // Clean up editor before base class destructor runs
    m_editor.reset();
}

JzRE::JzEditor &JzRE::JzREInstance::GetEditor()
{
    return *m_editor;
}

void JzRE::JzREInstance::OnStart()
{
    // Editor-specific initialization can be added here
}

void JzRE::JzREInstance::OnUpdate(F32 deltaTime)
{
    // Update editor logic (shortcuts, mode updates, panel updates)
    // This is called before UpdateSystems() in the main loop
    m_editor->Update(deltaTime);
}

void JzRE::JzREInstance::OnRender(F32 deltaTime)
{
    // Render editor views and UI
    // This is called after UpdateSystems() has run (camera, light, render preparation)
    // Rendering order:
    //   1. Game scene (rendered by RenderSystem during UpdateSystems)
    //   2. SceneView (captures game scene to framebuffer)
    //   3. ImGui UI (rendered on top)
    m_editor->Render(deltaTime);
}

void JzRE::JzREInstance::OnStop()
{
    // Editor-specific cleanup can be added here
}