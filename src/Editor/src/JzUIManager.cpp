/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzUIManager.h"
#include "JzRE/Editor/JzWindow.h"
#include "JzRE/UI/JzConverter.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

JzRE::JzUIManager::JzUIManager(JzWindow &window)
{
    ImGui::CreateContext();

    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

    SetDocking(false);

    ImGui_ImplGlfw_InitForOpenGL(window.GetGLFWWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 150");
    ImGui::StyleColorsDark();
}

JzRE::JzUIManager::~JzUIManager()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void JzRE::JzUIManager::Render()
{
    if (m_canvas) {
        m_canvas->Draw();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

void JzRE::JzUIManager::SetDocking(Bool value)
{
    m_dockingState = value;

    if (value) {
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    } else {
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
    }
}

JzRE::Bool JzRE::JzUIManager::IsDocking() const
{
    return m_dockingState;
}

void JzRE::JzUIManager::SetCanvas(JzCanvas &canvas)
{
    RemoveCanvas();

    m_canvas = &canvas;
}

void JzRE::JzUIManager::RemoveCanvas()
{
    m_canvas = nullptr;
}

void JzRE::JzUIManager::EnableEditorLayoutSave(JzRE::Bool value)
{
    if (value)
        ImGui::GetIO().IniFilename = m_layoutSaveFilename.c_str();
    else
        ImGui::GetIO().IniFilename = nullptr;
}

JzRE::Bool JzRE::JzUIManager::IsEditorLayoutSaveEnabled() const
{
    return ImGui::GetIO().IniFilename != nullptr;
}

void JzRE::JzUIManager::SetEditorLayoutSaveFilename(const JzRE::String &filename)
{
    m_layoutSaveFilename = filename;
    if (IsEditorLayoutSaveEnabled())
        ImGui::GetIO().IniFilename = m_layoutSaveFilename.c_str();
}

void JzRE::JzUIManager::ResetLayout(const JzRE::String &configPath) const
{
    ImGui::LoadIniSettingsFromDisk(configPath.c_str());
}

JzRE::Bool JzRE::JzUIManager::LoadFont(const JzRE::String &fontId, const JzRE::String &fontPath, JzRE::F32 fontSize)
{
    if (m_fonts.find(fontId) == m_fonts.end()) {
        auto   &io           = ImGui::GetIO();
        ImFont *fontInstance = io.Fonts->AddFontFromFileTTF(fontPath.c_str(),
                                                            fontSize,
                                                            nullptr,
                                                            io.Fonts->GetGlyphRangesChineseFull());
        if (fontInstance) {
            m_fonts[fontId] = fontInstance;
            return true;
        }
    }
    return false;
}

JzRE::Bool JzRE::JzUIManager::UnloadFont(const JzRE::String &fontId)
{
    if (m_fonts.find(fontId) != m_fonts.end()) {
        m_fonts.erase(fontId);
        return true;
    }
    return false;
}

JzRE::Bool JzRE::JzUIManager::UseFont(const JzRE::String &fontId)
{
    auto foundFont = m_fonts.find(fontId);
    if (foundFont != m_fonts.end()) {
        ImGui::GetIO().FontDefault = foundFont->second;
        return true;
    }
    return false;
}

void JzRE::JzUIManager::UseDefaultFont()
{
    ImGui::GetIO().FontDefault = nullptr;
}