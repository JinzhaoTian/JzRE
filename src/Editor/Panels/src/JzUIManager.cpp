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

    ApplyTheme();
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

void JzRE::JzUIManager::ApplyTheme()
{
    ImGuiStyle &style = ImGui::GetStyle();

    // Gray theme with good contrast
    // Text
    style.Colors[ImGuiCol_Text]         = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

    // Backgrounds
    style.Colors[ImGuiCol_WindowBg]  = ImVec4(0.18f, 0.18f, 0.18f, 0.95f);
    style.Colors[ImGuiCol_ChildBg]   = ImVec4(0.22f, 0.22f, 0.22f, 0.95f);
    style.Colors[ImGuiCol_PopupBg]   = ImVec4(0.25f, 0.25f, 0.25f, 0.95f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

    // Borders
    style.Colors[ImGuiCol_Border]       = ImVec4(0.35f, 0.35f, 0.35f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Title bar
    style.Colors[ImGuiCol_TitleBg]          = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]    = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);

    // Scrollbar
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);

    // Frame / Input
    style.Colors[ImGuiCol_FrameBg]        = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive]  = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);

    // Checkbox / Slider
    style.Colors[ImGuiCol_CheckMark]        = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]       = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);

    // Button
    style.Colors[ImGuiCol_Button]        = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]  = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);

    // Header (CollapsingHeader, TreeNode, etc.)
    style.Colors[ImGuiCol_Header]        = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]  = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

    // Separator
    style.Colors[ImGuiCol_Separator]        = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive]  = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);

    // Resize grip
    style.Colors[ImGuiCol_ResizeGrip]        = ImVec4(0.35f, 0.35f, 0.35f, 0.20f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.45f, 0.45f, 0.45f, 0.40f);
    style.Colors[ImGuiCol_ResizeGripActive]  = ImVec4(0.55f, 0.55f, 0.55f, 0.50f);

    // Tabs
    style.Colors[ImGuiCol_Tab]                 = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_TabHovered]          = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_TabActive]           = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_TabSelected]         = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused]        = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive]  = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);

    // Docking
    style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.35f, 0.35f, 0.35f, 0.50f);

    // Selection & Modal
    style.Colors[ImGuiCol_TextSelectedBg]   = ImVec4(0.40f, 0.40f, 0.40f, 0.50f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.60f);

    // Style adjustments
    style.WindowMenuButtonPosition = ImGuiDir_Right;
    style.WindowRounding           = 0.0f;
    style.FrameRounding            = 0.0f;
    style.ScrollbarRounding        = 0.0f;
    style.GrabRounding             = 0.0f;
    style.TabRounding              = 0.0f;
}
