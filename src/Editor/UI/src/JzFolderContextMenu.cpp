/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/UI/JzFolderContextMenu.h"
#include "JzRE/Editor/UI/JzAssetContextMenu.h"
#include "JzRE/Editor/UI/JzInputText.h"
#include "JzRE/Editor/UI/JzMenuList.h"
#include "JzRE/Runtime/Platform/JzMessageBox.h"

JzRE::JzFolderContextMenu::JzFolderContextMenu(const String &filePath) :
    JzAssetContextMenu(filePath) { }

void JzRE::JzFolderContextMenu::CreateList()
{
    auto &createMenu = CreateWidget<JzMenuList>("Create..");

    auto &_createFolderMenu               = createMenu.CreateWidget<JzMenuList>("Folder");
    auto &_createFolderInput              = _createFolderMenu.CreateWidget<JzInputText>("");
    _createFolderMenu.ClickedEvent       += [&_createFolderInput] { _createFolderInput.content = ""; };
    _createFolderInput.EnterPressedEvent += [this](String newFolderName) {
        const auto finalPath = std::filesystem::path(filePath / newFolderName);
        if (!std::filesystem::exists(finalPath)) {
            std::filesystem::create_directory(finalPath);
            ItemAddedEvent.Invoke(finalPath);
        }
        Close();
    };

    auto &_createFileMenu               = createMenu.CreateWidget<JzMenuList>("File");
    auto &_createFileInput              = _createFileMenu.CreateWidget<JzInputText>("");
    _createFileMenu.ClickedEvent       += [&_createFileInput] { _createFileInput.content = ""; };
    _createFileInput.EnterPressedEvent += [this](String newFileName) {
        const auto finalPath = std::filesystem::path(filePath / newFileName);
        if (!std::filesystem::exists(finalPath)) {
            // TODO
            // ItemAddedEvent.Invoke(finalPath);
        }
        Close();
    };
}

void JzRE::JzFolderContextMenu::DeleteItem()
{
    JzMessageBox message("Delete folder",
                         std::format("Are you sure that you want to delete \"{}\"?", filePath.string()),
                         JzEMessageBoxType::Warning,
                         JzEMessageBoxButton::YES_NO);

    if (message.GetUserAction() == JzEMessageBoxUserAction::Yes) {
        if (std::filesystem::exists(filePath) == true) {
            // EDITOR_EXEC(PropagateFolderDestruction(filePath.string()));
            // std::filesystem::remove_all(filePath);
            DestroyedEvent.Invoke(filePath);
        }
    }
}