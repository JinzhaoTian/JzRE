/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/Panels/JzUIManager.h"
#include "JzRE/Runtime/Function/ECS/JzWindowSystem.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

JzRE::JzUIManager::JzUIManager(JzWindowSystem &windowSystem)
{
    /* Setup Dear ImGui context */
    ImGui::CreateContext();

    ImGuiIO &io                          = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    SetDocking(false);

    /* Setup Platform/Renderer backends */
    auto *glfwWindow = static_cast<GLFWwindow *>(windowSystem.GetPlatformWindowHandle());
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    ImGui::StyleColorsDark();
}

JzRE::JzUIManager::~JzUIManager()
{
    /* Shutdown Dear ImGui context */
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void JzRE::JzUIManager::Render()
{
    if (m_canvas) {
        /* Start the Dear ImGui frame */

        // Prepare OpenGL state and resources
        ImGui_ImplOpenGL3_NewFrame();
        // Process mouse, keyboard, and other input
        ImGui_ImplGlfw_NewFrame();

        /* Prepare to draw */

        // Reset ImGui internal state
        ImGui::NewFrame();

        m_canvas->Draw();

        // Generate drawing data, calculate vertex, index and other data
        ImGui::Render();

        /* Actually Render */
        ImDrawData *draw_data = ImGui::GetDrawData();
        ImGui_ImplOpenGL3_RenderDrawData(draw_data);

        // (Option) For multi-viewport
        // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) { ... }
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