/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzREInstance.h"

JzRE::JzREInstance::JzREInstance(JzERHIType rhiType, std::filesystem::path &openDirectory) :
    JzRERuntime(rhiType, "JzRE", {1280, 720}),
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

void JzRE::JzREInstance::OnRender(F32 deltaTime)
{
    // Update and render editor UI (ImGui panels)
    m_editor->Update(deltaTime);
}

void JzRE::JzREInstance::OnStop()
{
    // Editor-specific cleanup can be added here
}

JzRE::Bool JzRE::JzREInstance::ShouldBlitToScreen() const
{
    // Editor uses ImGui to display the rendered framebuffer, so don't blit
    return false;
}
