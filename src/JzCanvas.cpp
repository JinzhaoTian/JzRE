/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzCanvas.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void JzRE::JzCanvas::Draw()
{
    if (!m_panels.empty()) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (m_isDockspace) {
            ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

            ImGui::Begin("##dockspace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking);
            ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
            ImGui::SetWindowPos({0.f, 0.f});
            ImVec2 displaySize = ImGui::GetIO().DisplaySize;
            ImGui::SetWindowSize({(float)displaySize.x, (float)displaySize.y});
            ImGui::End();

            ImGui::PopStyleVar(3);
        }

        for (auto &panel : m_panels) {
            panel.get().Draw();
        }

        ImGui::Render();
    }
}

void JzRE::JzCanvas::SetDockspace(Bool state)
{
    m_isDockspace = state;
}

JzRE::Bool JzRE::JzCanvas::IsDockspace() const
{
    return m_isDockspace;
}

void JzRE::JzCanvas::AddPanel(JzPanel &panel)
{
    m_panels.push_back(std::ref(panel));
}

void JzRE::JzCanvas::RemovePanel(JzPanel &panel)
{
    m_panels.erase(std::remove_if(m_panels.begin(), m_panels.end(), [&panel](std::reference_wrapper<JzRE::JzPanel> &p_item) {
        return &panel == &p_item.get();
    }));
}

void JzRE::JzCanvas::RemoveAllPanels()
{
    m_panels.clear();
}
