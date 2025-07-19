#include "OGLUI.h"

namespace JzRE {
OGLUI::OGLUI(GLFWwindow* window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.IniFilename = NULL;
    io.LogFilename = NULL;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // IF using Docking Branch - Not available in this ImGui version

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
}

OGLUI::~OGLUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void OGLUI::Render() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    Bool tool_active = true;
    ImGui::Begin("Tool Bar", &tool_active, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) {
                // TODO: Implement file dialog using nativefiledialog-extended or other vcpkg package
                // Set size for the dialog
                // ImGui::SetNextWindowSize(ImVec2(this->dialogWidth, this->dialogHeight), ImGuiCond_FirstUseEver);

                // IGFD::FileDialogConfig config;
                // config.path = ".";
                // ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".obj,.gltf,.glb", config);
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                /* Do stuff */
            }
            if (ImGui::MenuItem("Close", "Ctrl+W")) {
                tool_active = false;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text("This is some useful text.");

    ImGui::End();

    // display
    // TODO: Implement file dialog display using nativefiledialog-extended or other vcpkg package
    // if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) { // => will show a dialog
    //     if (ImGuiFileDialog::Instance()->IsOk()) {                  // action if OK
    //         String filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
    //         std::replace(filePathName.begin(), filePathName.end(), '\\', '/');

    //         if (callbackOpenFile) {
    //             callbackOpenFile(filePathName);
    //         }
    //     }

    //     // close
    //     ImGuiFileDialog::Instance()->Close();
    // }

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // ImGui::UpdatePlatformWindows(); // Not available in this ImGui version
}

void OGLUI::SetCallbackOpenFile(std::function<void(String)> callback) {
    this->callbackOpenFile = callback;
}

} // namespace JzRE