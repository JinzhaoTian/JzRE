#include "JzREHub.h"

JzRE::JzREHub::JzREHub()
{
    auto rhiType = JzERHIType::OpenGL;

    /* Settings */
    JzWindowSettings windowSettings;
    windowSettings.title       = "JzRE Hub";
    windowSettings.width       = 1000;
    windowSettings.height      = 580;
    windowSettings.isMaximized = false;
    windowSettings.isResizable = false;
    windowSettings.isDecorated = true;

    /* Window creation */
    m_window = std::make_unique<JzRE::JzWindow>(rhiType, windowSettings);
    m_window->MakeCurrentContext();

    /* Device */
    m_device = JzRHIFactory::CreateDevice(rhiType);

    m_uiManager = std::make_unique<JzUIManager>(m_window->GetGLFWWindow());
    m_uiManager->SetDocking(false);
}

JzRE::JzREHub::~JzREHub() { }

std::optional<std::filesystem::path> JzRE::JzREHub::Run()
{
    JzREHubPanel panel;

    m_uiManager->SetCanvas(m_canvas);
    m_canvas.AddPanel(panel);

    while (!m_window->ShouldClose()) {
        m_window->PollEvents();
        m_uiManager->Render();
        m_window->SwapBuffers();

        if (!panel.IsOpened()) {
            m_window->SetShouldClose(true);
        }
    }

    return panel.GetResult();
}

JzRE::JzREHubPanel::JzREHubPanel() :
    JzPanelWindow("JzRE Hub", true)
{
    resizable = false;
    movable   = false;
    titleBar  = false;

    auto &openProjectButton = CreateWidget<JzButton>("Open Project");
    auto &newProjectButton  = CreateWidget<JzButton>("New Project");
    auto &pathField         = CreateWidget<JzInputText>("");
    m_goButton              = &CreateWidget<JzButton>("GO");

    pathField.ContentChangedEvent += [this, &pathField](String p_content) {
        pathField.content = std::filesystem::path{p_content}.make_preferred().string();
        _UpdateGoButton(pathField.content);
    };

    _UpdateGoButton({});

    openProjectButton.ClickedEvent += [this] {
        // JzOpenFileDialog dialog("Open project");
        // dialog.AddFileType("Project", "*.*");
        // dialog.Show();

        // const std::filesystem::path projectFile   = dialog.GetSelectedFilePath();
        // const std::filesystem::path projectFolder = projectFile.parent_path();

        // if (dialog.HasSucceeded()) {
        //     if (!_TryFinish({projectFolder})) {
        //         _OnFailedToOpenCorruptedProject(projectFolder);
        //     }
        // }
    };

    newProjectButton.ClickedEvent += [this, &pathField] {
        // JzSaveFileDialog dialog("New project location");
        // dialog.DefineExtension("Project", "..");
        // dialog.Show();

        // if (dialog.HasSucceeded()) {
        //     String selectedFile = dialog.GetSelectedFilePath();

        //     if (selectedFile.ends_with("..")) {
        //         selectedFile.erase(selectedFile.size() - 2);
        //     }

        //     pathField.content = selectedFile;

        //     _UpdateGoButton(pathField.content);
        // }
    };

    m_goButton->ClickedEvent += [this, &pathField] {
        // const std::filesystem::path path = pathField.content;

        // if (!Utils::ProjectManagement::CreateProject(path) || !_TryFinish({path})) {
        //     _OnFailedToCreateProject(path);
        // }
    };

    openProjectButton.lineBreak = false;
    newProjectButton.lineBreak  = false;
    pathField.lineBreak         = false;

    CreateWidget<JzSpacing>();
    CreateWidget<JzSeparator>();
    CreateWidget<JzSpacing>();

    // auto &columns = CreateWidget<OvUI::Widgets::Layout::Columns<2>>();

    // columns.widths = {750, 500};

    // Sanitize the project registry before displaying it, so we avoid showing
    // corrupted/deleted projects.
    // Utils::ProjectManagement::SanitizeProjectRegistry();
    // const auto registeredProjects = Utils::ProjectManagement::GetRegisteredProjects();

    // for (const auto &project : registeredProjects) {
    //     auto &text         = columns.CreateWidget<OvUI::Widgets::Texts::Text>(project.string());
    //     auto &actions      = columns.CreateWidget<OvUI::Widgets::Layout::Group>();
    //     auto &openButton   = actions.CreateWidget<OvUI::Widgets::Buttons::Button>("Open");
    //     auto &deleteButton = actions.CreateWidget<OvUI::Widgets::Buttons::Button>("Delete");

    //     openButton.idleBackgroundColor   = {0.7f, 0.5f, 0.f};
    //     deleteButton.idleBackgroundColor = {0.5f, 0.f, 0.f};

    //     openButton.ClickedEvent += [this, &text, &actions, project] {
    //         if (!_TryFinish({project})) {
    //             text.Destroy();
    //             actions.Destroy();
    //             _OnFailedToOpenCorruptedProject(project);
    //         }
    //     };

    //     deleteButton.ClickedEvent += [this, &text, &actions, project] {
    //         text.Destroy();
    //         actions.Destroy();
    //         Utils::ProjectManagement::UnregisterProject(project);
    //     };

    //     openButton.lineBreak = false;
    //     deleteButton.lineBreak;
    // }
}

std::optional<std::filesystem::path> JzRE::JzREHubPanel::GetResult() const
{
    return m_result;
}

void JzRE::JzREHubPanel::Draw()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {50.f, 50.f});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);

    JzPanelWindow::Draw();

    ImGui::PopStyleVar(2);
}

void JzRE::JzREHubPanel::_UpdateGoButton(const JzRE::String &p_path)
{
    const Bool validPath = !p_path.empty();
    m_goButton->disabled = !validPath;
    // m_goButton->idleBackgroundColor = validPath ? OvUI::Types::Color{0.f, 0.5f, 0.0f} : OvUI::Types::Color{0.1f, 0.1f, 0.1f};
}

void JzRE::JzREHubPanel::_OnFailedToOpenCorruptedProject(const std::filesystem::path &p_projectPath)
{
    // TODO: Implement project management system
    // Utils::ProjectManagement::UnregisterProject(p_projectPath);

    _ShowError(
        "Invalid project",
        "The selected project is invalid or corrupted.\nPlease select another project.");
}

void JzRE::JzREHubPanel::_OnFailedToCreateProject(const std::filesystem::path &p_projectPath)
{
    _ShowError(
        "Project creation failed",
        "Something went wrong while creating the project.\nPlease ensure the path is valid and you have the necessary permissions.");
}

void JzRE::JzREHubPanel::_ShowError(const JzRE::String &p_title, const JzRE::String &p_message)
{
    // TODO: Implement proper error dialog system
    // For now, just print to console
    printf("Error: %s - %s\n", p_title.c_str(), p_message.c_str());
}

JzRE::Bool JzRE::JzREHubPanel::_ValidateResult(const std::filesystem::path &p_result)
{
    // TODO: Implement proper project validation
    // For now, just check if path exists and is a directory
    return std::filesystem::exists(p_result) && std::filesystem::is_directory(p_result);
}

JzRE::Bool JzRE::JzREHubPanel::_TryFinish(const std::filesystem::path p_result)
{
    if (_ValidateResult(p_result)) {
        m_result = p_result;
        Close();
        return true;
    }

    return false;
}