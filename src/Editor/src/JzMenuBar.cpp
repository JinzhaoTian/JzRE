/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzMenuBar.h"
#include <filesystem>
#include <imgui.h>
#include "JzRE/Core/JzServiceContainer.h"
#include "JzRE/UI/JzGroup.h"
#include "JzRE/UI/JzEAlignment.h"
#include "JzRE/UI/JzText.h"
#include "JzRE/UI/JzArrowButton.h"
#include "JzRE/UI/JzSeparator.h"
#include "JzRE/UI/JzConverter.h"
#include "JzRE/Editor/JzSceneManager.h"
#include "JzRE/Editor/JzInputManager.h"
#include "JzRE/Platform/JzOpenFileDialog.h"

JzRE::JzMenuBar::JzMenuBar(JzRE::JzWindow &window) :
    m_window(window)
{
    CreateFileMenu();
    // CreateBuildMenu();
    CreateWindowMenu();
    // CreateActorsMenu();
    // CreateResourcesMenu();
    // CreateToolsMenu();
    CreateSettingsMenu();
    // CreateLayoutMenu();
    // CreateHelpMenu();
    CreateRunButton();
}

void JzRE::JzMenuBar::HandleShortcuts(F32 deltaTime)
{
    auto &inputMgr = JzServiceContainer::Get<JzInputManager>();

    if (inputMgr.GetKeyState(JzEInputKey::KEY_LEFT_CONTROL) == JzEInputKeyState::KEY_DOWN) {
        if (inputMgr.IsKeyPressed(JzEInputKey::KEY_N)) {
            auto &sceneMgr = JzServiceContainer::Get<JzSceneManager>();
            sceneMgr.LoadDefaultScene();
        }

        if (inputMgr.IsKeyPressed(JzEInputKey::KEY_S)) {
            if (inputMgr.GetKeyState(JzEInputKey::KEY_LEFT_SHIFT) == JzEInputKeyState::KEY_UP) {
                // EDITOR_EXEC(SaveSceneChanges());
            } else {
                // EDITOR_EXEC(SaveAs());
            }
        }
    }
}

void JzRE::JzMenuBar::RegisterPanel(const String &name, JzPanelWindow &panel)
{
    auto &menuItem              = m_windowMenu->CreateWidget<JzMenuItem>(name, "", true, true);
    menuItem.ValueChangedEvent += std::bind(&JzPanelWindow::SetOpened, &panel, std::placeholders::_1);

    m_panels.emplace(name, std::make_pair(std::ref(panel), std::ref(menuItem)));
}

void JzRE::JzMenuBar::InitializeSettingsMenu()
{
    auto &themeButton = m_settingsMenu->CreateWidget<JzMenuList>("Editor Theme");
    themeButton.CreateWidget<JzText>("Some themes may require a restart");

    auto &cameraPositionMenu  = m_settingsMenu->CreateWidget<JzMenuList>("Reset Camera");
    auto &viewColors          = m_settingsMenu->CreateWidget<JzMenuList>("View Colors");
    auto &sceneViewBackground = viewColors.CreateWidget<JzMenuList>("Scene View Background");
    auto &sceneViewGrid       = viewColors.CreateWidget<JzMenuList>("Scene View Grid");
    sceneViewGrid.CreateWidget<JzMenuItem>("Reset");

    auto &assetViewBackground = viewColors.CreateWidget<JzMenuList>("Asset View Background");
    assetViewBackground.CreateWidget<JzMenuItem>("Reset");

    auto &consoleSettingsMenu = m_settingsMenu->CreateWidget<JzMenuList>("Console Settings");
}

void JzRE::JzMenuBar::_Draw_Impl()
{
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, JzConverter::HexToImVec4(m_backgroudColor));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    if (!m_widgets.empty() && ImGui::BeginMainMenuBar()) {
        DrawWidgets();
        ImGui::EndMainMenuBar();
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void JzRE::JzMenuBar::CreateFileMenu()
{
    auto &fileMenu = CreateWidget<JzMenuList>("File");

    auto &openFileMenu         = fileMenu.CreateWidget<JzMenuItem>("Open File", "CTRL + O");
    openFileMenu.ClickedEvent += [] {
        JzOpenFileDialog dialog("Open File");
        dialog.AddFileType("*", "*.*");
        dialog.Show(JzEFileDialogType::OpenFile);

        const std::filesystem::path projectFile = dialog.GetSelectedFilePath();

        if (dialog.HasSucceeded()) {
            // TODO
        }
    };

    auto &openFileFolderMenu         = fileMenu.CreateWidget<JzMenuItem>("Open File Folder", "CTRL + K CTRL + O");
    openFileFolderMenu.ClickedEvent += [] {
        JzOpenFileDialog dialog("Open Folder");
        dialog.AddFileType("*", "*.*");
        dialog.Show(JzEFileDialogType::OpenFolder);

        const std::filesystem::path projectFile = dialog.GetSelectedFilePath();

        if (dialog.HasSucceeded()) {
            // TODO
        }
    };

    fileMenu.CreateWidget<JzSeparator>();

    auto &closeFileFolderMenu         = fileMenu.CreateWidget<JzMenuItem>("Close File Folder", "CTRL + K F");
    closeFileFolderMenu.ClickedEvent += [] { };

    fileMenu.CreateWidget<JzSeparator>();

    auto &exitMenu         = fileMenu.CreateWidget<JzMenuItem>("Exit", "ALT + F4");
    exitMenu.ClickedEvent += [this] { m_window.SetShouldClose(true); };
}

void JzRE::JzMenuBar::CreateBuildMenu()
{
    auto &buildMenu = CreateWidget<JzMenuList>("Build");
}

void JzRE::JzMenuBar::CreateWindowMenu()
{
    m_windowMenu                                                      = &CreateWidget<JzMenuList>("Window");
    m_windowMenu->CreateWidget<JzMenuItem>("Close all").ClickedEvent += std::bind(&JzMenuBar::OpenEveryWindows, this, false);
    m_windowMenu->CreateWidget<JzMenuItem>("Open all").ClickedEvent  += std::bind(&JzMenuBar::OpenEveryWindows, this, true);
    m_windowMenu->CreateWidget<JzSeparator>();

    /* When the menu is opened, we update which window is marked as "Opened" or "Closed" */
    m_windowMenu->ClickedEvent += std::bind(&JzMenuBar::UpdateToggleableItems, this);
}

void JzRE::JzMenuBar::CreateActorsMenu() { }

void JzRE::JzMenuBar::CreateResourcesMenu()
{
    auto &resourcesMenu = CreateWidget<JzMenuList>("Resources");
}

void JzRE::JzMenuBar::CreateToolsMenu()
{
    auto &toolsMenu = CreateWidget<JzMenuList>("Tools");
}

void JzRE::JzMenuBar::CreateSettingsMenu()
{
    m_settingsMenu = &CreateWidget<JzMenuList>("Settings");
}

void JzRE::JzMenuBar::CreateLayoutMenu()
{
    auto &layoutMenu = CreateWidget<JzMenuList>("Layout");
}

void JzRE::JzMenuBar::CreateHelpMenu()
{
    auto &helpMenu = CreateWidget<JzMenuList>("Help");
    helpMenu.CreateWidget<JzText>("JzRE: " + std::string("test"));
}

void JzRE::JzMenuBar::CreateRunButton()
{
    auto &action    = CreateWidget<JzGroup>(JzEHorizontalAlignment::CENTER);
    auto &runButton = action.CreateWidget<JzArrowButton>(JzEArrowDirection::RIGHT);
}

void JzRE::JzMenuBar::UpdateToggleableItems()
{
    for (auto &[name, panel] : m_panels)
        panel.second.get().checked = panel.first.get().IsOpened();
}

void JzRE::JzMenuBar::OpenEveryWindows(JzRE::Bool state)
{
    for (auto &[name, panel] : m_panels)
        panel.first.get().SetOpened(state);
}