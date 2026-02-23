/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/UI/JzAssetContextMenu.h"
#include "JzRE/Editor/UI/JzInputText.h"
#include "JzRE/Editor/UI/JzMenuItem.h"
#include "JzRE/Editor/UI/JzMenuList.h"

constexpr std::string_view kAllowedFilenameChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-_=+ 0123456789()[]";

JzRE::JzAssetContextMenu::JzAssetContextMenu(const String filePath) :
    filePath(filePath) { }

void JzRE::JzAssetContextMenu::CreateList()
{
    auto &deleteAction         = CreateWidget<JzMenuItem>("Delete");
    deleteAction.ClickedEvent += [this] { DeleteItem(); };

    auto &renameMenu             = CreateWidget<JzMenuList>("Rename to...");
    auto &_nameInput             = renameMenu.CreateWidget<JzInputText>("");
    _nameInput.selectAllOnClick  = true;
    renameMenu.ClickedEvent     += [this, &_nameInput] {
        _nameInput.content = filePath.stem().string();

        if (!std::filesystem::is_directory(filePath)) {
            if (Size pos = _nameInput.content.rfind('.'); pos != std::string::npos) {
                _nameInput.content = _nameInput.content.substr(0, pos);
            }
        }
    };
    _nameInput.EnterPressedEvent += [this](String p_newName) {
        if (!std::filesystem::is_directory(filePath)) {
            p_newName += filePath.extension().string();
        }

        // Remove non-allowed characters.
        std::erase_if(p_newName, [](char c) {
            return kAllowedFilenameChars.find(c) >= kAllowedFilenameChars.size();
        });

        const std::filesystem::path parentFolder = std::filesystem::path{filePath}.parent_path();
        const std::filesystem::path newPath      = parentFolder / p_newName;
        const std::filesystem::path oldPath      = filePath;

        if (filePath != newPath && !std::filesystem::exists(newPath)) {
            filePath = newPath;
        }

        RenamedEvent.Invoke(oldPath, newPath);
    };
}

void JzRE::JzAssetContextMenu::Execute(JzEPluginExecutionContext context)
{
    if (m_widgets.size() > 0) {
        JzContextMenu::Execute(context);
    }
}
