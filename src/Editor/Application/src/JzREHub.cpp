/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzREHub.h"
#include <algorithm>
#include <cctype>
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
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Resource/JzTexture.h"
#include "JzRE/Editor/UI/JzButton.h"
#include "JzRE/Editor/UI/JzIconButton.h"
#include "JzRE/Editor/UI/JzGroup.h"
#include "JzRE/Editor/UI/JzText.h"
#include "JzRE/Editor/UI/JzInputText.h"
#include "JzRE/Editor/UI/JzSpacing.h"
#include "JzRE/Editor/UI/JzSeparator.h"
#include "JzRE/Editor/UI/JzInputText.h"
#include "JzRE/Editor/UI/JzColumns.h"
#include "JzRE/Editor/UI/JzConverter.h"
#include "JzRE/Runtime/Platform/Dialog/JzOpenFileDialog.h"
#include "JzRE/Runtime/Platform/RHI/JzGraphicsContext.h"
#include "JzRE/Runtime/Function/Project/JzProjectManager.h"

JzRE::JzREHub::JzREHub(JzERHIType rhiType)
{
    JzServiceContainer::Init();

    JzWindowConfig windowConfig;
    windowConfig.title     = "JzRE Hub";
    windowConfig.width     = 800;
    windowConfig.height    = 500;
    windowConfig.resizable = false;
    windowConfig.decorated = false;

    m_windowSystem = std::make_unique<JzWindowSystem>();
    m_windowSystem->InitializeWindow(rhiType, windowConfig);
    m_windowSystem->SetAlignCentered();

    m_graphicsContext   = std::make_unique<JzGraphicsContext>();
    auto *windowBackend = m_windowSystem->GetBackend();
    if (windowBackend) {
        m_graphicsContext->Initialize(*windowBackend, rhiType);
        JzServiceContainer::Provide<JzGraphicsContext>(*m_graphicsContext);
        JzServiceContainer::Provide<JzDevice>(m_graphicsContext->GetDevice());
    }

    m_uiManager = std::make_unique<JzUIManager>(*m_windowSystem);

    const auto fontPath = std::filesystem::current_path() / "fonts" / "SourceHanSansCN-Regular.otf";
    m_uiManager->LoadFont("sourcehansanscn-regular-16", fontPath.string(), 16);
    m_uiManager->UseFont("sourcehansanscn-regular-16");
    m_uiManager->EnableEditorLayoutSave(false);
    m_uiManager->SetDocking(false);

    m_canvas = std::make_unique<JzCanvas>();

    m_menuBar = std::make_unique<JzREHubMenuBar>(*m_windowSystem);
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

    if (m_graphicsContext) {
        JzServiceContainer::Remove<JzGraphicsContext>();
        JzServiceContainer::Remove<JzDevice>();
        m_graphicsContext->Shutdown();
        m_graphicsContext.reset();
    }

    if (m_windowSystem) {
        m_windowSystem.reset();
    }
}

std::optional<std::filesystem::path> JzRE::JzREHub::Run()
{
    while (!m_windowSystem->ShouldClose()) {
        m_windowSystem->PollWindowEvents();
        m_uiManager->Render();
        if (m_graphicsContext) {
            m_graphicsContext->Present();
        }

        if (!m_hubPanel->IsOpened()) {
            m_windowSystem->SetShouldClose(true);
        }
    }

    return m_hubPanel->GetResult();
}

JzRE::JzREHubMenuBar::JzREHubMenuBar(JzRE::JzWindowSystem &windowSystem) :
    m_windowSystem(windowSystem),
    m_buttonSize({30.0f, 20.0f}),
    m_backgroudColor("#2A2A2A"),
    m_isDragging(false)
{
    const auto iconsDir = std::filesystem::current_path() / "icons";

    auto &actions = CreateWidget<JzGroup>(JzEHorizontalAlignment::RIGHT, JzVec2(80.f, 0.f), JzVec2(0.f, 0.f));

    auto minimizeIcon = std::make_shared<JzTexture>((iconsDir / "minimize-64.png").string());
    minimizeIcon->Load();
    auto &minimizeButton            = actions.CreateWidget<JzIconButton>(minimizeIcon->GetRhiTexture());
    minimizeButton.buttonSize       = m_buttonSize;
    minimizeButton.buttonIdleColor  = m_backgroudColor;
    minimizeButton.lineBreak        = false;
    minimizeButton.ClickedEvent    += [this]() {
        if (m_windowSystem.IsMinimized())
            m_windowSystem.Restore();
        else
            m_windowSystem.Minimize();
    };

    auto maximizeIcon = std::make_shared<JzTexture>((iconsDir / "maximize-64.png").string());
    maximizeIcon->Load();
    auto &maximizeButton            = actions.CreateWidget<JzIconButton>(maximizeIcon->GetRhiTexture());
    maximizeButton.buttonSize       = m_buttonSize;
    maximizeButton.buttonIdleColor  = m_backgroudColor;
    maximizeButton.lineBreak        = false;
    maximizeButton.ClickedEvent    += [this]() {
        if (m_windowSystem.IsFullscreen())
            m_windowSystem.SetFullscreen(false);
        else
            m_windowSystem.SetFullscreen(true);
    };

    auto closeIcon = std::make_shared<JzTexture>((iconsDir / "close-64.png").string());
    closeIcon->Load();
    auto &closeButton               = actions.CreateWidget<JzIconButton>(closeIcon->GetRhiTexture());
    closeButton.buttonSize          = m_buttonSize;
    closeButton.buttonIdleColor     = m_backgroudColor;
    closeButton.buttonHoveredColor  = "#e81123";
    closeButton.buttonClickedColor  = "#ec6c77";
    closeButton.iconSize            = {14.f, 14.f};
    closeButton.lineBreak           = true;
    closeButton.ClickedEvent       += [this]() { m_windowSystem.SetShouldClose(true); };
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

    const JzIVec2 windowScreenPos       = m_windowSystem.GetPosition();
    const JzIVec2 currentMouseScreenPos = windowScreenPos + JzIVec2(io.MousePos.x, io.MousePos.y);

    if (isMouseOverMenuBar && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        m_dragStartWindowPos = windowScreenPos;
        m_dragStartMousePos  = currentMouseScreenPos;
        m_isDragging         = true;
    }

    if (m_isDragging) {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            const JzIVec2 delta = currentMouseScreenPos - m_dragStartMousePos;
            m_windowSystem.SetPosition(m_dragStartWindowPos + delta);
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

    auto &searchBar                = CreateWidget<JzInputText>("");
    searchBar.width                = m_inputFieldWidth;
    searchBar.lineBreak            = false;
    searchBar.tooltip              = "Search projects...";
    searchBar.ContentChangedEvent += [this](const String &searchText) {
        _FilterHistory(searchText);
    };

    auto &openButton             = CreateWidget<JzButton>("Open Project");
    openButton.buttonIdleColor   = "#e3c79f";
    openButton.buttonSize        = m_buttonSize;
    openButton.buttonLabelColor  = "#003153";
    openButton.lineBreak         = false;
    openButton.ClickedEvent     += [this] {
        JzOpenFileDialog dialog("Open Project");
        dialog.AddFileType("JzRE Project", "*.jzreproject");
        dialog.Show(JzEFileDialogType::OpenFile);

        const std::filesystem::path openPath = dialog.GetSelectedFilePath();

        if (dialog.HasSucceeded()) {
            if (!_OnFinish({openPath})) {
                _OnFailedToOpenPath(openPath);
            }
        }
    };

    auto &createButton             = CreateWidget<JzButton>("Create Project");
    createButton.buttonIdleColor   = "#d3674d";
    createButton.buttonSize        = m_buttonSize;
    createButton.buttonLabelColor  = "#003153";
    createButton.lineBreak         = true;
    createButton.ClickedEvent     += [this] {
        JzOpenFileDialog dialog("Select Project Folder");
        dialog.AddFileType("*", "*.*");
        dialog.Show(JzEFileDialogType::OpenFolder);

        if (dialog.HasSucceeded()) {
            const std::filesystem::path projectDir = dialog.GetSelectedFilePath();

            // Use folder name as project name
            const String projectName = projectDir.filename().string();

            JzProjectManager projectManager;
            auto             result = projectManager.CreateProject(projectDir, projectName);

            if (result == JzEProjectResult::Success) {
                _OnFinish(projectDir);
            } else {
                JzRE_LOG_ERROR("Failed to create project at: {}", projectDir.string());
            }
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

        m_historyEntries.push_back({path, &_text, &_actions});
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

    JzVec2 padding = {std::max((panelSize.x - contentSize.x) * 0.5f, 0.0f), 50.f};

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
    return JzRE::String(reinterpret_cast<const char *>(path.generic_u8string().c_str()));
}

std::filesystem::path JzRE::JzREHubPanel::_Utf8ToPath(const JzRE::String &utf8Str) const
{
    return std::filesystem::path(utf8Str);
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

void JzRE::JzREHubPanel::_FilterHistory(const JzRE::String &searchText)
{
    for (auto &entry : m_historyEntries) {
        if (entry.textWidget && entry.actionsWidget) {
            const Bool visible           = searchText.empty() || _FuzzyMatch(_PathToUtf8(entry.path), searchText);
            entry.textWidget->enabled    = visible;
            entry.actionsWidget->enabled = visible;
        }
    }
}

JzRE::Bool JzRE::JzREHubPanel::_FuzzyMatch(const JzRE::String &text, const JzRE::String &pattern) const
{
    // Case-insensitive fuzzy matching
    String lowerText    = text;
    String lowerPattern = pattern;

    std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    std::transform(lowerPattern.begin(), lowerPattern.end(), lowerPattern.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    // Check if pattern characters appear in order in text
    Size patternIdx = 0;
    for (Size textIdx = 0; textIdx < lowerText.size() && patternIdx < lowerPattern.size(); ++textIdx) {
        if (lowerText[textIdx] == lowerPattern[patternIdx]) {
            ++patternIdx;
        }
    }

    return patternIdx == lowerPattern.size();
}
