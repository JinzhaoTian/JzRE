/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/Panels/JzMenuBar.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Editor/UI/JzGroup.h"
#include "JzRE/Editor/UI/JzEAlignment.h"
#include "JzRE/Editor/UI/JzText.h"
#include "JzRE/Editor/UI/JzArrowButton.h"
#include "JzRE/Editor/UI/JzSeparator.h"
#include "JzRE/Editor/UI/JzConverter.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/ECS/JzInputComponents.h"
#include "JzRE/Runtime/Function/ECS/JzWindowComponents.h"
#include "JzRE/Runtime/Platform/JzOpenFileDialog.h"

JzRE::JzMenuBar::JzMenuBar(JzRE::JzWindowSystem &windowSystem) :
    m_windowSystem(windowSystem)
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
    auto &world = JzServiceContainer::Get<JzWorld>();

    // Get primary window input state
    JzInputStateComponent *inputState = nullptr;
    auto                   inputView  = world.View<JzInputStateComponent, JzPrimaryWindowTag>();
    for (auto entity : inputView) {
        inputState = &world.GetComponent<JzInputStateComponent>(entity);
        break;
    }
    if (!inputState) return;

    const auto &input = *inputState;

    if (input.keyboard.IsKeyPressed(JzEKeyCode::LeftControl)) {
        if (input.keyboard.IsKeyDown(JzEKeyCode::N)) {
            // TODO: New scene
        }

        if (input.keyboard.IsKeyDown(JzEKeyCode::S)) {
            if (!input.keyboard.IsKeyPressed(JzEKeyCode::LeftShift)) {
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
        dialog.AddFileType("OBJ Files", "*.obj");
        dialog.AddFileType("All Files", "*.*");
        dialog.Show(JzEFileDialogType::OpenFile);

        const std::filesystem::path filePath = dialog.GetSelectedFilePath();

        if (dialog.HasSucceeded() && !filePath.empty()) {
            // Get the file extension and check if it's an OBJ file
            String extension = filePath.extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

            if (extension == ".obj") {
                // TODO: Migrate to EnTT ECS
            }
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
    exitMenu.ClickedEvent += [this] { m_windowSystem.SetShouldClose(true); };
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