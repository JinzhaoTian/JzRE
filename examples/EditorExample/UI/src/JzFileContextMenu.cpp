/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/UI/JzFileContextMenu.h"
#include "JzRE/Editor/UI/JzMenuItem.h"
#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/Asset/JzAssetExporter.h"
#include "JzRE/Runtime/Platform/Dialog/JzMessageBox.h"
#include "JzRE/Runtime/Platform/Dialog/JzOpenFileDialog.h"

JzRE::JzFileContextMenu::JzFileContextMenu(const JzRE::String &filePath) :
    JzAssetContextMenu(filePath) { }

void JzRE::JzFileContextMenu::CreateList()
{
    auto &openAction         = CreateWidget<JzMenuItem>("Open");
    openAction.ClickedEvent += [this] {
        // TODO OpenFile(filePath.string());
    };

    auto &copyAction         = CreateWidget<JzMenuItem>("Copy");
    copyAction.ClickedEvent += [this] {
        // const auto finalPath = FindAvailableFilePath(filePath);
        // std::filesystem::copy(filePath, finalPath);
        // DuplicateEvent.Invoke(finalPath);
    };

    auto &exportAction         = CreateWidget<JzMenuItem>("Export...");
    exportAction.ClickedEvent += [this] {
        JzOpenFileDialog dialog("Export To Folder");
        dialog.Show(JzEFileDialogType::OpenFolder);

        if (dialog.HasSucceeded()) {
            auto &exporter = JzServiceContainer::Get<JzAssetExporter>();
            auto  result   = exporter.ExportFile(filePath, dialog.GetSelectedFilePath());

            if (result.result == JzEExportResult::Success) {
                JzRE_LOG_INFO("Asset exported: {}", result.destinationPath.string());
            } else {
                JzRE_LOG_ERROR("Asset export failed: {}", result.errorMessage);
            }
        }
    };

    JzAssetContextMenu::CreateList();

    auto &propertiesAction         = CreateWidget<JzMenuItem>("Properties");
    propertiesAction.ClickedEvent += [this] {
        // TODO
    };
}

void JzRE::JzFileContextMenu::DeleteItem()
{
    JzMessageBox message("Delete file",
                         std::format("Are you sure that you want to delete \"{}\"?", filePath.string()),
                         JzEMessageBoxType::Warning,
                         JzEMessageBoxButton::YES_NO);

    if (message.GetUserAction() == JzEMessageBoxUserAction::Yes) {
        if (std::filesystem::exists(filePath) == true) {
            // RemoveAsset(filePath.string());
            // EDITOR_EXEC(PropagateFileRename(filePath.string(), "?"));
            DestroyedEvent.Invoke(filePath);
        }
    }
}