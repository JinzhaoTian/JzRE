/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzREEditor.h"

namespace {

JzRE::JzRERuntimeSettings CreateSettingsFromPath(JzRE::JzERHIType             rhiType,
                                                 const std::filesystem::path &openPath)
{
    JzRE::JzRERuntimeSettings settings;
    settings.windowTitle = "JzRE";
    settings.windowSize  = {1280, 720};
    settings.rhiType     = rhiType;

    // Check if path is a project file or directory
    if (!openPath.empty()) {
        auto projectFile = JzRE::JzREEditor::FindProjectFile(openPath);
        if (!projectFile.empty()) {
            settings.projectFile = projectFile;
        }
    }

    return settings;
}

} // anonymous namespace

std::filesystem::path JzRE::JzREEditor::FindProjectFile(const std::filesystem::path &path)
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
            if (entry.is_regular_file() && entry.path().extension() == JzProjectManager::GetProjectFileExtension()) {
                return entry.path();
            }
        }
    }

    return {};
}

JzRE::JzREEditor::JzREEditor(JzERHIType rhiType, const std::filesystem::path &openPath) :
    JzRERuntime(CreateSettingsFromPath(rhiType, openPath)),
    m_openPath(openPath)
{
    // Create editor UI with runtime reference
    m_editorUI = std::make_unique<JzEditorUI>(*this);
}

JzRE::JzREEditor::~JzREEditor()
{
    // Clean up editor UI before base class destructor runs
    m_editorUI.reset();
}

JzRE::JzEditorUI &JzRE::JzREEditor::GetEditorUI()
{
    return *m_editorUI;
}

void JzRE::JzREEditor::OnStart()
{
    // Editor-specific initialization can be added here
}

void JzRE::JzREEditor::OnUpdate(F32 deltaTime)
{
    // Update editor logic (shortcuts, mode updates, panel updates)
    // This is called before UpdateSystems() in the main loop
    m_editorUI->Update(deltaTime);
}

void JzRE::JzREEditor::OnRender(F32 deltaTime)
{
    // Render editor views and UI
    // This is called after UpdateSystems() has run (camera, light, render preparation)
    // Rendering order:
    //   1. Game scene (rendered by RenderSystem during UpdateSystems)
    //   2. SceneView (captures game scene to framebuffer)
    //   3. ImGui UI (rendered on top)
    m_editorUI->Render(deltaTime);
}

void JzRE::JzREEditor::OnStop()
{
    // Editor-specific cleanup can be added here
}