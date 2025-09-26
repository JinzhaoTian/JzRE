/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/App/JzREHub.h"
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
#include "JzRE/Core/JzServiceContainer.h"
#include "JzRE/Core/JzLogger.h"
#include "JzRE/RHI/JzRHIDevice.h"
#include "JzRE/RHI/JzRHIFactory.h"
#include "JzRE/Resource/JzTexture.h"
#include "JzRE/Resource/JzTextureFactory.h"
#include "JzRE/Platform/JzOpenFileDialog.h"
#include "JzRE/UI/JzIconButton.h"
#include "JzRE/UI/JzGroup.h"
#include "JzRE/UI/JzText.h"
#include "JzRE/UI/JzInputText.h"
#include "JzRE/UI/JzSpacing.h"
#include "JzRE/UI/JzSeparator.h"
#include "JzRE/UI/JzColumns.h"
#include "JzRE/UI/JzConverter.h"

JzRE::JzREHub::JzREHub(JzERHIType rhiType)
{
    JzWindowSettings windowSettings;
    windowSettings.title       = "JzRE Hub";
    windowSettings.size        = {800, 500};
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

    m_menuBar = std::make_unique<JzREHubMenuBar>(*m_window);
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

JzRE::JzREHubMenuBar::JzREHubMenuBar(JzRE::JzWindow &window) :
    m_window(window),
    m_buttonSize({30.0f, 20.0f}),
    m_backgroudColor("#2A2A2A"),
    m_isDragging(false)
{
    auto &resourceManager = JzServiceContainer::Get<JzResourceManager>();

    auto &actions = CreateWidget<JzGroup>(JzEHorizontalAlignment::RIGHT, JzVec2(80.f, 0.f), JzVec2(0.f, 0.f));

    auto  minimizeIcon              = resourceManager.GetResource<JzTexture>("icons/minimize-64.png");
    auto &minimizeButton            = actions.CreateWidget<JzIconButton>(minimizeIcon->GetRhiTexture());
    minimizeButton.buttonSize       = m_buttonSize;
    minimizeButton.buttonIdleColor  = m_backgroudColor;
    minimizeButton.lineBreak        = false;
    minimizeButton.ClickedEvent    += [this]() {
        if (m_window.IsMinimized())
            m_window.Restore();
        else
            m_window.Minimize();
    };

    auto  maximizeIcon              = resourceManager.GetResource<JzTexture>("icons/maximize-64.png");
    auto &maximizeButton            = actions.CreateWidget<JzIconButton>(maximizeIcon->GetRhiTexture());
    maximizeButton.buttonSize       = m_buttonSize;
    maximizeButton.buttonIdleColor  = m_backgroudColor;
    maximizeButton.lineBreak        = false;
    maximizeButton.ClickedEvent    += [this]() {
        if (m_window.IsFullscreen())
            m_window.SetFullscreen(false);
        else
            m_window.SetFullscreen(true);
    };

    auto  closeIcon                 = resourceManager.GetResource<JzTexture>("icons/close-64.png");
    auto &closeButton               = actions.CreateWidget<JzIconButton>(closeIcon->GetRhiTexture());
    closeButton.buttonSize          = m_buttonSize;
    closeButton.buttonIdleColor     = m_backgroudColor;
    closeButton.buttonHoveredColor  = "#e81123";
    closeButton.buttonClickedColor  = "#ec6c77";
    closeButton.iconSize            = {14.f, 14.f};
    closeButton.lineBreak           = true;
    closeButton.ClickedEvent       += [this]() { m_window.SetShouldClose(true); };
}

void JzRE::JzREHubMenuBar::_Draw_Impl()
{
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, JzConverter::HexToImVec4(m_backgroudColor));
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

    auto &pathField                = CreateWidget<JzInputText>("");
    pathField.width                = m_inputFieldWidth;
    pathField.lineBreak            = false;
    pathField.ContentChangedEvent += [this, &pathField](String p_content) {
        pathField.content = std::filesystem::path{p_content}.make_preferred().string();
        _OnUpdateGoButton(pathField.content);
    };

    auto &openButton             = CreateWidget<JzButton>("Open Folder");
    openButton.buttonIdleColor   = "#e3c79f";
    openButton.buttonSize        = m_buttonSize;
    openButton.buttonLabelColor  = "#003153";
    openButton.lineBreak         = false;
    openButton.ClickedEvent     += [this] {
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

    m_goButton                   = &CreateWidget<JzButton>("GO");
    m_goButton->buttonIdleColor  = "#36373a";
    m_goButton->buttonSize       = m_buttonSize;
    m_goButton->disabled         = true;
    m_goButton->lineBreak        = true;
    m_goButton->ClickedEvent    += [this, &pathField] {
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

        auto &_openBtn            = _actions.CreateWidget<JzButton>("Open");
        _openBtn.buttonIdleColor  = "#003153";
        _openBtn.buttonSize       = m_buttonSize;
        _openBtn.lineBreak        = false;
        _openBtn.ClickedEvent    += [this, &_text, &_actions, path] {
            if (!_OnFinish(path)) {
                _text.Destroy();
                _actions.Destroy();
                _DeleteFromHistory(path);
                _OnFailedToOpenPath(path);
            }
        };

        auto &_deleteBtn            = _actions.CreateWidget<JzButton>("Delete");
        _deleteBtn.buttonIdleColor  = "#b5120f";
        _deleteBtn.buttonSize       = m_buttonSize;
        _deleteBtn.lineBreak        = true;
        _deleteBtn.ClickedEvent    += [this, &_text, &_actions, path] {
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
    const ImGuiViewport *viewport = ImGui::GetMainViewport();

    const JzVec2 panelPos  = {viewport->WorkPos.x, viewport->WorkPos.y};
    const JzVec2 panelSize = {viewport->WorkSize.x, viewport->WorkSize.y};

    SetPosition(panelPos);
    SetSize(panelSize);

    const JzVec2 contentSize = _CalculateContentSize();

    JzVec2 padding = {std::max((panelSize.x() - contentSize.x()) * 0.5f, 0.0f), 50.f};

    ImGui::PushStyleColor(ImGuiCol_WindowBg, JzConverter::HexToImVec4(m_backgroudColor));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, JzConverter::ToImVec2(padding));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    JzPanelWindow::Draw();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(1);
}

JzRE::JzVec2 JzRE::JzREHubPanel::_CalculateContentSize()
{
    const F32 contentWidth = 700.0f;

    F32 contentHeight = 0.0f;

    const ImGuiStyle &style           = ImGui::GetStyle();
    const F32         frameHeight     = ImGui::GetFrameHeightWithSpacing();
    const F32         separatorHeight = style.ItemSpacing.y * 4;

    contentHeight += frameHeight;

    contentHeight += separatorHeight;

    if (!m_history.empty()) {
        contentHeight += m_history.size() * frameHeight;
    }

    return {contentWidth, contentHeight};
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

        file << jsonObj.dump(4);
    } catch (const std::exception &e) {
        // TODO
    }

    file.close();
}

void JzRE::JzREHubPanel::_AddToHistory(const std::filesystem::path &path)
{
    auto it = std::find(m_history.begin(), m_history.end(), path);
    if (it != m_history.end()) {
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
    const Bool validPath        = !p_path.empty();
    m_goButton->disabled        = !validPath;
    m_goButton->buttonIdleColor = validPath ? "#26bbff" : "#36373a";
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