/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/UI/JzFileContextMenu.h"
#include "JzRE/UI/JzMenuItem.h"
#include "JzRE/Platform/JzMessageBox.h"

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
                         JzEMessageBoxType::WARNING,
                         JzEMessageBoxButton::YES_NO);

    if (message.GetUserAction() == JzEMessageBoxUserAction::Yes) {
        if (std::filesystem::exists(filePath) == true) {
            // RemoveAsset(filePath.string());
            // EDITOR_EXEC(PropagateFileRename(filePath.string(), "?"));
            DestroyedEvent.Invoke(filePath);
        }
    }
}