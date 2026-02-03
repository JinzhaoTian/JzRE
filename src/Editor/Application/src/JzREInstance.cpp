/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzREInstance.h"

JzRE::JzREInstance::JzREInstance(JzERHIType rhiType, std::filesystem::path &openDirectory) :
    JzRERuntime(JzRERuntimeSettings{"JzRE", {1280, 720}, rhiType}),
    m_openDirectory(openDirectory)
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