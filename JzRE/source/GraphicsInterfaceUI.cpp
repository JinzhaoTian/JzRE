#include "GraphicsInterfaceUI.h"

namespace JzRE {
GraphicsInterfaceUI::GraphicsInterfaceUI(RawPtr<GLFWwindow> window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.IniFilename = NULL;
    io.LogFilename = NULL;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
}

GraphicsInterfaceUI::~GraphicsInterfaceUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GraphicsInterfaceUI::Render() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Show a simple window
    ImGui::Begin("Hello, world!");
    ImGui::Text("This is some useful text.");
    ImGui::End();

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    ImGui::UpdatePlatformWindows();
}

} // namespace JzRE