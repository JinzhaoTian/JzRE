/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzREHub.h"
#include <fstream>
#include <memory>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif
#include <nlohmann/json.hpp>
#include "JzServiceContainer.h"
#include "JzRHIDevice.h"
#include "JzLogger.h"
#include "JzRHIFactory.h"
#include "JzTexture.h"
#include "JzTextureFactory.h"
#include "JzOpenFileDialog.h"
#include "JzIconButton.h"
#include "JzGroup.h"
#include "JzText.h"
#include "JzInputText.h"
#include "JzSpacing.h"
#include "JzSeparator.h"
#include "JzColumns.h"

JzRE::JzREHub::JzREHub(JzERHIType rhiType)
{
    JzWindowSettings windowSettings;
    windowSettings.title       = "JzRE Hub";
    windowSettings.x           = 50;
    windowSettings.y           = 50;
    windowSettings.width       = 800;
    windowSettings.height      = 500;
    windowSettings.isResizable = false;
    windowSettings.isDecorated = false;

    m_window = std::make_unique<JzWindow>(rhiType, windowSettings);
    m_window->MakeCurrentContext();
    m_window->SetAlignCentered();

    m_device = JzRHIFactory::CreateDevice(rhiType);
    JzServiceContainer::Provide<JzRHIDevice>(*m_device);

    m_resourceManager = std::make_unique<JzResourceManager>();
    m_resourceManager->RegisterFactory<JzTexture>(std::make_unique<JzTextureFactory>());
    m_resourceManager->AddSearchPath("./icons");
    JzServiceContainer::Provide<JzResourceManager>(*m_resourceManager);

    m_uiManager = std::make_unique<JzUIManager>(*m_window);

    const auto fontPath = std::filesystem::current_path() / "fonts" / "SourceHanSansCN-Regular.otf";
    m_uiManager->LoadFont("sourcehansanscn-regular-16", fontPath.string(), 16);
    m_uiManager->UseFont("sourcehansanscn-regular-16");
    m_uiManager->EnableEditorLayoutSave(false);
    m_uiManager->SetDocking(false);

    m_canvas = std::make_unique<JzCanvas>();

    m_menuBar = std::make_unique<JzREHubMenuBar>(*m_window, *m_resourceManager);
    m_canvas->AddPanel(*m_menuBar);

    m_hubPanel = std::make_unique<JzREHubPanel>();
    m_canvas->AddPanel(*m_hubPanel);

    m_uiManager->SetCanvas(*m_canvas);
}

JzRE::JzREHub::~JzREHub()
{
    if (m_hubPanel) {
        m_hubPanel.reset();
    }

    if (m_canvas) {
        m_canvas.reset();
    }

    if (m_uiManager) {
        m_uiManager.reset();
    }

    if (m_resourceManager) {
        m_resourceManager.reset();
    }

    if (m_device) {
        m_device.reset();
    }

    if (m_window) {
        m_window.reset();
    }
}

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

JzRE::JzREHubMenuBar::JzREHubMenuBar(JzRE::JzWindow &window, JzRE::JzResourceManager &resourceManager) :
    m_window(window),
    m_resourceManager(resourceManager)
{
    auto &actions = CreateWidget<JzGroup>(JzEHorizontalAlignment::RIGHT, JzVec2(80.f, 0.f), JzVec2(0.f, 0.f));

    auto  minimizeIcon                  = resourceManager.GetResource<JzTexture>("icons/window-minimize.png");
    auto &minimizeButton                = actions.CreateWidget<JzIconButton>(minimizeIcon->GetRhiTexture());
    minimizeButton.idleBackgroundColor  = {0.1333f, 0.1529f, 0.1804f, 1.0f};
    minimizeButton.buttonSize           = m_buttonSize;
    minimizeButton.iconColor            = {1.f, 1.f, 1.f, 1.f};
    minimizeButton.iconSize             = m_iconSize;
    minimizeButton.lineBreak            = false;
    minimizeButton.ClickedEvent        += [this]() {
        if (m_window.IsMinimized())
            m_window.Restore();
        else
            m_window.Minimize();
    };

    auto  maximizeIcon                  = resourceManager.GetResource<JzTexture>("icons/window-maximize.png");
    auto &maximizeButton                = actions.CreateWidget<JzIconButton>(maximizeIcon->GetRhiTexture());
    maximizeButton.idleBackgroundColor  = {0.1333f, 0.1529f, 0.1804f, 1.0f};
    maximizeButton.buttonSize           = m_buttonSize;
    maximizeButton.iconColor            = {1.f, 1.f, 1.f, 1.f};
    maximizeButton.iconSize             = m_iconSize;
    maximizeButton.lineBreak            = false;
    maximizeButton.ClickedEvent        += [this]() {
        if (m_window.IsMaximized())
            m_window.Restore();
        else
            m_window.Maximize();
    };

    auto  closeIcon                  = resourceManager.GetResource<JzTexture>("icons/x.png");
    auto &closeButton                = actions.CreateWidget<JzIconButton>(closeIcon->GetRhiTexture());
    closeButton.idleBackgroundColor  = {0.1333f, 0.1529f, 0.1804f, 1.0f};
    closeButton.buttonSize           = m_buttonSize;
    closeButton.iconColor            = {1.f, 1.f, 1.f, 1.f};
    closeButton.iconSize             = m_iconSize;
    closeButton.lineBreak            = true;
    closeButton.ClickedEvent        += [this]() { m_window.SetShouldClose(true); };
}

void JzRE::JzREHubMenuBar::_Draw_Impl()
{
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.1333f, 0.1529f, 0.1804f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    if (!m_widgets.empty() && ImGui::BeginMainMenuBar()) {
        HandleDragging();

        DrawWidgets();

        ImGui::EndMainMenuBar();
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void JzRE::JzREHubMenuBar::HandleDragging()
{
    ImVec2 menuBarMin = ImGui::GetWindowPos();
    ImVec2 menuBarMax = ImVec2(menuBarMin.x + ImGui::GetWindowWidth(), menuBarMin.y + ImGui::GetWindowHeight());

    ImGuiIO &io                 = ImGui::GetIO();
    Bool     isMouseOverMenuBar = ImGui::IsMouseHoveringRect(menuBarMin, menuBarMax);

    const JzIVec2 windowScreenPos       = m_window.GetPosition();
    const JzIVec2 currentMouseScreenPos = windowScreenPos + JzIVec2(io.MousePos.x, io.MousePos.y);

    if (isMouseOverMenuBar && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        m_dragStartWindowPos = windowScreenPos;
        m_dragStartMousePos  = currentMouseScreenPos;
        m_isDragging         = true;
    }

    if (m_isDragging) {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            const JzIVec2 delta = currentMouseScreenPos - m_dragStartMousePos;
            m_window.SetPosition(m_dragStartWindowPos + delta);
        }
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            m_isDragging = false;
        }
    }
}

JzRE::JzREHubPanel::JzREHubPanel() :
    JzPanelWindow("JzRE Hub", true),
    m_workspaceFilePath(std::filesystem::current_path() / "config" / "workspace.json")
{
    resizable = false;
    movable   = false;
    titleBar  = false;

    SetSize(m_windowSize);
    SetPosition(m_windowPosition);

    auto &pathField                = CreateWidget<JzInputText>("");
    pathField.width                = m_inputFieldWidth;
    pathField.lineBreak            = false;
    pathField.ContentChangedEvent += [this, &pathField](String p_content) {
        pathField.content = std::filesystem::path{p_content}.make_preferred().string();
        _OnUpdateGoButton(pathField.content);
    };

    auto &openButton                = CreateWidget<JzButton>("Open Folder");
    openButton.idleBackgroundColor  = {0.7f, 0.5f, 0.0f, 1.0f};
    openButton.size                 = m_buttonSize;
    openButton.lineBreak            = false;
    openButton.ClickedEvent        += [this] {
        JzOpenFileDialog dialog("Open Folder");
        dialog.AddFileType("*", "*.*");
        dialog.Show(JzEFileDialogType::OpenFolder);

        const std::filesystem::path openPath = dialog.GetSelectedFilePath();

        if (dialog.HasSucceeded()) {
            if (!_OnFinish({openPath})) {
                _OnFailedToOpenPath(openPath);
            }
        }
    };

    m_goButton                       = &CreateWidget<JzButton>("GO");
    m_goButton->idleBackgroundColor  = {0.1f, 0.1f, 0.1f, 1.0f};
    m_goButton->size                 = m_buttonSize;
    m_goButton->disabled             = true;
    m_goButton->lineBreak            = true;
    m_goButton->ClickedEvent        += [this, &pathField] {
        const std::filesystem::path path = pathField.content;

        if (!_OnFinish({path})) {
            _OnFailedToOpenPath(path);
        }
    };

    CreateWidget<JzSpacing>(2);
    CreateWidget<JzSeparator>();
    CreateWidget<JzSpacing>(2);

    auto &columns  = CreateWidget<JzColumns<2>>();
    columns.widths = {512.0f, 200.0f};

    _LoadHistory();

    for (const auto &path : m_history) {
        auto &_text = columns.CreateWidget<JzText>(_PathToUtf8(path));

        auto &_actions = columns.CreateWidget<JzGroup>();

        auto &_openBtn                = _actions.CreateWidget<JzButton>("Open");
        _openBtn.idleBackgroundColor  = {0.7f, 0.5f, 0.0f, 1.0f};
        _openBtn.size                 = m_buttonSize;
        _openBtn.lineBreak            = false;
        _openBtn.ClickedEvent        += [this, &_text, &_actions, path] {
            if (!_OnFinish(path)) {
                _text.Destroy();
                _actions.Destroy();
                _DeleteFromHistory(path);
                _OnFailedToOpenPath(path);
            }
        };

        auto &_deleteBtn                = _actions.CreateWidget<JzButton>("Delete");
        _deleteBtn.idleBackgroundColor  = {0.3f, 0.0f, 0.0f, 1.0f};
        _deleteBtn.size                 = {90.0f, 0.0f};
        _deleteBtn.lineBreak            = true;
        _deleteBtn.ClickedEvent        += [this, &_text, &_actions, path] {
            _text.Destroy();
            _actions.Destroy();
            _DeleteFromHistory(path);
        };
    }
}

JzRE::JzREHubPanel::~JzREHubPanel()
{
    _SaveHistory();
}

std::optional<std::filesystem::path> JzRE::JzREHubPanel::GetResult() const
{
    return m_result;
}

void JzRE::JzREHubPanel::Draw()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1333f, 0.1529f, 0.1804f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {50.0f, 50.0f});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    JzPanelWindow::Draw();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(1);
}

void JzRE::JzREHubPanel::_LoadHistory()
{
    m_history.clear();

    std::filesystem::create_directories(m_workspaceFilePath.parent_path());

    std::ifstream file(m_workspaceFilePath);
    if (!file.is_open()) {
        return;
    }

    try {
        nlohmann::json jsonObj;
        file >> jsonObj;

        if (jsonObj.contains("lastOpenFiles") && jsonObj["lastOpenFiles"].is_array()) {
            for (const auto &item : jsonObj["lastOpenFiles"]) {
                if (item.is_string()) {
                    auto utf8Path = item.get<JzRE::String>();
                    m_history.push_back(_Utf8ToPath(utf8Path));

                    // 限制历史记录数量
                    if (m_history.size() >= m_maxHistorySize) {
                        break;
                    }
                }
            }
        }
    } catch (const std::exception &e) {
        // TODO
    }

    file.close();
}

void JzRE::JzREHubPanel::_SaveHistory()
{
    std::filesystem::create_directories(m_workspaceFilePath.parent_path());

    std::ofstream file(m_workspaceFilePath);
    if (!file.is_open()) {
        return;
    }

    try {
        nlohmann::json jsonObj;
        jsonObj["lastOpenFiles"] = nlohmann::json::array();

        for (const auto &path : m_history) {
            jsonObj["lastOpenFiles"].push_back(_PathToUtf8(path));
        }

        file << jsonObj.dump(4); // 使用4空格缩进美化输出
    } catch (const std::exception &e) {
        // TODO
    }

    file.close();
}

void JzRE::JzREHubPanel::_AddToHistory(const std::filesystem::path &path)
{
    auto it = std::find(m_history.begin(), m_history.end(), path);
    if (it != m_history.end()) {
        // 如果已存在，移动到最前面
        m_history.erase(it);
    }

    m_history.insert(m_history.begin(), path);

    if (m_history.size() > m_maxHistorySize) {
        m_history.pop_back();
    }

    _SaveHistory();
}

void JzRE::JzREHubPanel::_DeleteFromHistory(const std::filesystem::path &path)
{
    auto it = std::find(m_history.begin(), m_history.end(), path);
    if (it != m_history.end()) {
        m_history.erase(it);
    }

    _SaveHistory();
}

JzRE::String JzRE::JzREHubPanel::_PathToUtf8(const std::filesystem::path &path) const
{
    auto u8str = path.generic_u8string();
    return JzRE::String(u8str.begin(), u8str.end());
}

std::filesystem::path JzRE::JzREHubPanel::_Utf8ToPath(const JzRE::String &utf8Str) const
{
    return std::filesystem::path(std::string_view(utf8Str.data(), utf8Str.size()));
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

JzRE::Bool JzRE::JzREHubPanel::_OnFinish(const std::filesystem::path path)
{
    if (!std::filesystem::exists(path)) {
        JzRE_LOG_ERROR("Path is not existed.");
        return false;
    }

    _AddToHistory(path);

    m_result = path;
    Close();
    return true;
}