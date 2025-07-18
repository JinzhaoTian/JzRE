#include "JzUIManager.h"

JzRE::JzUIManager::JzUIManager(RawPtr<GLFWwindow> glfwWindow, const String &glslVersion)
{
    ImGui::CreateContext();

    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

    SetDocking(false);

    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    ImGui_ImplOpenGL3_Init(glslVersion.c_str());

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
