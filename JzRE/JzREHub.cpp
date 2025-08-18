#include "JzREHub.h"

JzRE::JzREHub::JzREHub()
{
    auto rhiType = JzERHIType::OpenGL;

    /* Settings */
    JzWindowSettings windowSettings;
    windowSettings.title       = "JzRE Hub";
    windowSettings.x           = 50;
    windowSettings.y           = 50;
    windowSettings.width       = 800;
    windowSettings.height      = 500;
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

    /* JzRE Hub Panel */
    m_hubPanel = std::make_unique<JzREHubPanel>();

    m_uiManager->SetCanvas(m_canvas);
    m_canvas.AddPanel(*m_hubPanel);
}

JzRE::JzREHub::~JzREHub() { }

std::optional<std::filesystem::path> JzRE::JzREHub::Run()
{
    while (!m_window->ShouldClose()) {
        m_window->PollEvents();
        m_uiManager->Render();
        m_window->SwapBuffers();

        if (!m_hubPanel->IsOpened()) {
            m_window->SetShouldClose(true);
        }
    }

    return m_hubPanel->GetResult();
}

JzRE::JzREHubPanel::JzREHubPanel() :
    JzPanelWindow("JzRE Hub", true)
{
    resizable = false;
    movable   = false;
    titleBar  = false;

    SetSize({800.0f, 500.0f});
    SetPosition({0.0f, 0.0f});

    auto &openButton                = CreateWidget<JzButton>("Open Folder");
    openButton.idleBackgroundColor  = {0.7f, 0.5f, 0.0f, 1.0f};
    openButton.size                 = {90.0f, 0.0f};
    openButton.lineBreak            = false;
    openButton.ClickedEvent        += [this] {
        JzOpenFileDialog dialog("Open Floder");
        dialog.AddFileType("*", "*.*");
        dialog.Show();

        const std::filesystem::path projectFile   = dialog.GetSelectedFilePath();
        const std::filesystem::path projectFolder = projectFile.parent_path();

        if (dialog.HasSucceeded()) {
            if (!_OnFinish({projectFolder})) {
                _OnFailedToOpenPath(projectFolder);
            }
        }
    };

    auto &pathField                = CreateWidget<JzInputText>("");
    pathField.width                = 504.0f;
    pathField.lineBreak            = false;
    pathField.ContentChangedEvent += [this, &pathField](String p_content) {
        pathField.content = std::filesystem::path{p_content}.make_preferred().string();
        _OnUpdateGoButton(pathField.content);
    };

    m_goButton                       = &CreateWidget<JzButton>("GO");
    m_goButton->idleBackgroundColor  = {0.1f, 0.1f, 0.1f, 1.0f};
    m_goButton->size                 = {90.0f, 0.0f};
    m_goButton->disabled             = true;
    m_goButton->lineBreak            = true;
    m_goButton->ClickedEvent        += [this, &pathField] {
        const std::filesystem::path path = pathField.content;

        if (!_OnFinish({path})) {
            _OnFailedToOpenPath(path);
        }
    };

    CreateWidget<JzSpacing>();
    CreateWidget<JzSeparator>();
    CreateWidget<JzSpacing>();

    auto &columns  = CreateWidget<JzColumns<2>>();
    columns.widths = {512.0f, 200.0f};

    auto pathes = std::vector<String>{"sda", "sd", "snba"}; // TODO

    for (const auto &path : pathes) {
        auto &_text = columns.CreateWidget<JzText>(path);

        auto &_actions = columns.CreateWidget<JzGroup>();

        auto &_openBtn                = _actions.CreateWidget<JzButton>("Open");
        _openBtn.idleBackgroundColor  = {0.7f, 0.5f, 0.0f, 1.0f};
        _openBtn.size                 = {90.0f, 0.0f};
        _openBtn.lineBreak            = false;
        _openBtn.ClickedEvent        += [this, &_text, &_actions, path] {
            if (!_OnFinish(path)) {
                _text.Destroy();
                _actions.Destroy();
                _OnFailedToOpenPath(path);
            }
        };

        auto &_deleteBtn                = _actions.CreateWidget<JzButton>("Delete");
        _deleteBtn.idleBackgroundColor  = {0.5f, 0.0f, 0.0f, 1.0f};
        _deleteBtn.size                 = {90.0f, 0.0f};
        _deleteBtn.lineBreak            = true;
        _deleteBtn.ClickedEvent        += [this, &_text, &_actions, path] {
            _text.Destroy();
            _actions.Destroy();
        };
    }
}

std::optional<std::filesystem::path> JzRE::JzREHubPanel::GetResult() const
{
    return m_result;
}

void JzRE::JzREHubPanel::Draw()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {50.0f, 50.0f});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    JzPanelWindow::Draw();

    ImGui::PopStyleVar(2);
}

void JzRE::JzREHubPanel::_OnUpdateGoButton(const JzRE::String &p_path)
{
    const Bool validPath            = !p_path.empty();
    m_goButton->disabled            = !validPath;
    m_goButton->idleBackgroundColor = validPath ? JzVec4(0.0f, 0.5f, 0.0f, 1.0f) : JzVec4(0.1f, 0.1f, 0.1f, 1.0f);
}

void JzRE::JzREHubPanel::_OnFailedToOpenPath(const std::filesystem::path &p_path)
{
    // TODO
}

JzRE::Bool JzRE::JzREHubPanel::_OnFinish(const std::filesystem::path p_result)
{
    m_result = p_result;
    Close();
    return true;
}