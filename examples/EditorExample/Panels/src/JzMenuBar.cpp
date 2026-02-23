/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/Panels/JzMenuBar.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Resource/JzTexture.h"
#include "JzRE/Editor/UI/JzGroup.h"
#include "JzRE/Editor/UI/JzEAlignment.h"
#include "JzRE/Editor/UI/JzText.h"
#include "JzRE/Editor/UI/JzArrowButton.h"
#include "JzRE/Editor/UI/JzIconButton.h"
#include "JzRE/Editor/UI/JzSeparator.h"
#include "JzRE/Editor/UI/JzConverter.h"
#include "JzRE/Editor/Core/JzEditorState.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/ECS/JzInputComponents.h"
#include "JzRE/Runtime/Function/ECS/JzWindowComponents.h"
#include "JzRE/Runtime/Function/Asset/JzAssetImporter.h"
#include "JzRE/Runtime/Function/Asset/JzAssetExporter.h"
#include "JzRE/Runtime/Function/Project/JzProjectManager.h"
#include "JzRE/Runtime/Function/Scene/JzSceneSerializer.h"
#include "JzRE/Runtime/Platform/Dialog/JzOpenFileDialog.h"
#include "JzRE/Runtime/Platform/Dialog/JzSaveFileDialog.h"

JzRE::JzMenuBar::JzMenuBar(JzRE::JzWindowSystem &windowSystem) :
    m_windowSystem(windowSystem)
{
    CreateFileMenu();
    // CreateBuildMenu();
    CreateWindowMenu();
    // CreateActorsMenu();
    CreateResourcesMenu();
    // CreateToolsMenu();
    CreateSettingsMenu();
    // CreateLayoutMenu();
    // CreateHelpMenu();
    CreateRunButton();
    CreateWindowActions();
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

        if (input.keyboard.IsKeyDown(JzEKeyCode::I)) {
            if (JzServiceContainer::Has<JzProjectManager>() &&
                JzServiceContainer::Get<JzProjectManager>().HasLoadedProject()) {
                JzOpenFileDialog dialog("Import Asset");
                auto filters = JzAssetImporter::GetSupportedFileFilters();
                for (const auto &[label, filter] : filters) {
                    dialog.AddFileType(label, filter);
                }
                dialog.AddFileType("All Files", "*.*");
                dialog.Show(JzEFileDialogType::OpenFile);

                if (dialog.HasSucceeded()) {
                    auto &importer = JzServiceContainer::Get<JzAssetImporter>();
                    importer.ImportFile(dialog.GetSelectedFilePath());
                }
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

    auto &cameraPositionMenu = m_settingsMenu->CreateWidget<JzMenuList>("Reset Camera");
    auto &viewColors         = m_settingsMenu->CreateWidget<JzMenuList>("View Colors");
    auto &sceneViewBackground = viewColors.CreateWidget<JzMenuList>("Scene View Background");
    auto &sceneViewGrid       = viewColors.CreateWidget<JzMenuList>("Scene View Grid");
    sceneViewGrid.CreateWidget<JzMenuItem>("Reset");

    auto &assetViewBackground = viewColors.CreateWidget<JzMenuList>("Asset View Background");
    assetViewBackground.CreateWidget<JzMenuItem>("Reset");

    auto &consoleSettingsMenu = m_settingsMenu->CreateWidget<JzMenuList>("Console Settings");
    (void)cameraPositionMenu;
    (void)sceneViewBackground;
    (void)consoleSettingsMenu;

    // SceneView rendering helper toggles
    if (JzServiceContainer::Has<JzEditorState>()) {
        auto &editorState = JzServiceContainer::Get<JzEditorState>();

        auto &sceneViewMenu = m_settingsMenu->CreateWidget<JzMenuList>("Scene View");

        auto &skyboxItem = sceneViewMenu.CreateWidget<JzMenuItem>(
            "Show Skybox", "", true, editorState.sceneSkyboxEnabled);
        skyboxItem.ValueChangedEvent += [&editorState](Bool enabled) {
            editorState.SetSceneSkyboxEnabled(enabled);
        };

        auto &axisItem = sceneViewMenu.CreateWidget<JzMenuItem>(
            "Show Axis", "", true, editorState.sceneAxisEnabled);
        axisItem.ValueChangedEvent += [&editorState](Bool enabled) {
            editorState.SetSceneAxisEnabled(enabled);
        };

        auto &gridItem = sceneViewMenu.CreateWidget<JzMenuItem>(
            "Show Grid", "", true, editorState.sceneGridEnabled);
        gridItem.ValueChangedEvent += [&editorState](Bool enabled) {
            editorState.SetSceneGridEnabled(enabled);
        };
    }
}

void JzRE::JzMenuBar::_Draw_Impl()
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

void JzRE::JzMenuBar::CreateFileMenu()
{
    auto &fileMenu = CreateWidget<JzMenuList>("File");

    // Scene operations
    auto &newSceneMenu         = fileMenu.CreateWidget<JzMenuItem>("New Scene", "CTRL + N");
    newSceneMenu.ClickedEvent += [] {
        if (!JzServiceContainer::Has<JzWorld>()) return;
        auto &world = JzServiceContainer::Get<JzWorld>();
        JzSceneSerializer::ClearScene(world);
    };

    auto &openSceneMenu         = fileMenu.CreateWidget<JzMenuItem>("Open Scene...", "CTRL + O");
    openSceneMenu.ClickedEvent += [] {
        if (!JzServiceContainer::Has<JzWorld>()) return;

        JzOpenFileDialog dialog("Open Scene");
        dialog.AddFileType("JzRE Scene", "*.jzscene");
        dialog.AddFileType("All Files", "*.*");
        dialog.Show(JzEFileDialogType::OpenFile);

        if (dialog.HasSucceeded()) {
            auto &world = JzServiceContainer::Get<JzWorld>();
            JzSceneSerializer::ClearScene(world);
            JzSceneSerializer::Deserialize(world, dialog.GetSelectedFilePath());
        }
    };

    auto &saveSceneMenu         = fileMenu.CreateWidget<JzMenuItem>("Save Scene...", "CTRL + S");
    saveSceneMenu.ClickedEvent += [] {
        if (!JzServiceContainer::Has<JzWorld>()) return;

        JzSaveFileDialog dialog("Save Scene");
        dialog.DefineExtension("JzRE Scene", ".jzscene");
        dialog.Show(JzEFileDialogType::SaveFile);

        if (dialog.HasSucceeded()) {
            auto &world = JzServiceContainer::Get<JzWorld>();
            auto filepath = dialog.GetSelectedFilePath();
            JzSceneSerializer::Serialize(world, filepath);
        }
    };

    fileMenu.CreateWidget<JzSeparator>();

    auto &openFileMenu         = fileMenu.CreateWidget<JzMenuItem>("Open File", "");
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

    auto &importAssetMenu         = fileMenu.CreateWidget<JzMenuItem>("Import Asset...", "CTRL + I");
    importAssetMenu.ClickedEvent += [] {
        if (!JzServiceContainer::Has<JzProjectManager>() ||
            !JzServiceContainer::Get<JzProjectManager>().HasLoadedProject()) {
            return;
        }

        JzOpenFileDialog dialog("Import Asset");
        auto filters = JzAssetImporter::GetSupportedFileFilters();
        for (const auto &[label, filter] : filters) {
            dialog.AddFileType(label, filter);
        }
        dialog.AddFileType("All Files", "*.*");
        dialog.Show(JzEFileDialogType::OpenFile);

        if (dialog.HasSucceeded()) {
            auto &importer = JzServiceContainer::Get<JzAssetImporter>();
            importer.ImportFile(dialog.GetSelectedFilePath());
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

    auto &importItem         = resourcesMenu.CreateWidget<JzMenuItem>("Import Asset...");
    importItem.ClickedEvent += [] {
        if (!JzServiceContainer::Has<JzProjectManager>() ||
            !JzServiceContainer::Get<JzProjectManager>().HasLoadedProject()) {
            return;
        }

        JzOpenFileDialog dialog("Import Asset");
        auto filters = JzAssetImporter::GetSupportedFileFilters();
        for (const auto &[label, filter] : filters) {
            dialog.AddFileType(label, filter);
        }
        dialog.AddFileType("All Files", "*.*");
        dialog.Show(JzEFileDialogType::OpenFile);

        if (dialog.HasSucceeded()) {
            auto &importer = JzServiceContainer::Get<JzAssetImporter>();
            importer.ImportFile(dialog.GetSelectedFilePath());
        }
    };

    auto &exportItem         = resourcesMenu.CreateWidget<JzMenuItem>("Export Asset...");
    exportItem.ClickedEvent += [] {
        if (!JzServiceContainer::Has<JzProjectManager>() ||
            !JzServiceContainer::Get<JzProjectManager>().HasLoadedProject()) {
            return;
        }

        JzOpenFileDialog dialog("Select Export Destination");
        dialog.Show(JzEFileDialogType::OpenFolder);

        if (dialog.HasSucceeded()) {
            auto        &projectManager = JzServiceContainer::Get<JzProjectManager>();
            const auto   contentPath    = projectManager.GetConfig().GetContentPath();
            auto        &exporter       = JzServiceContainer::Get<JzAssetExporter>();

            // Export all files from the Content directory
            std::vector<std::filesystem::path> assetPaths;
            if (std::filesystem::exists(contentPath)) {
                for (const auto &entry : std::filesystem::recursive_directory_iterator(contentPath)) {
                    if (entry.is_regular_file()) {
                        assetPaths.push_back(entry.path());
                    }
                }
            }

            if (!assetPaths.empty()) {
                exporter.ExportFiles(assetPaths, dialog.GetSelectedFilePath());
            }
        }
    };
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

void JzRE::JzMenuBar::CreateWindowActions()
{
    const auto iconsDir = std::filesystem::current_path() / "icons";

    auto &actions = CreateWidget<JzGroup>(JzEHorizontalAlignment::RIGHT, JzVec2(80.f, 0.f), JzVec2(0.f, 0.f));

    auto minimizeIcon = std::make_shared<JzTexture>((iconsDir / "minimize-64.png").string());
    minimizeIcon->Load();
    auto &minimizeButton            = actions.CreateWidget<JzIconButton>(minimizeIcon->GetRhiTexture());
    minimizeButton.buttonSize       = m_actionButtonSize;
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
    maximizeButton.buttonSize       = m_actionButtonSize;
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
    closeButton.buttonSize          = m_actionButtonSize;
    closeButton.buttonIdleColor     = m_backgroudColor;
    closeButton.buttonHoveredColor  = "#e81123";
    closeButton.buttonClickedColor  = "#ec6c77";
    closeButton.iconSize            = {14.f, 14.f};
    closeButton.lineBreak           = true;
    closeButton.ClickedEvent       += [this]() { m_windowSystem.SetShouldClose(true); };
}

void JzRE::JzMenuBar::HandleDragging()
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
