/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/UI/JzModelContextMenu.h"
#include "JzRE/Editor/UI/JzMenuItem.h"
#include "JzRE/Editor/UI/JzMenuList.h"

JzRE::JzModelContextMenu::JzModelContextMenu(const JzRE::String &filePath) :
    JzFileContextMenu(filePath) { }

void JzRE::JzModelContextMenu::CreateList()
{
    auto &reloadAction         = CreateWidget<JzMenuItem>("Reload");
    reloadAction.ClickedEvent += [this] {
        // TODO
    };

    auto &generateMaterialsMenu = CreateWidget<JzMenuList>(
        "Generate materials...");

    // CreateMaterialCreationOption(generateMaterialsMenu, "Standard");
    // CreateMaterialCreationOption(generateMaterialsMenu, "Unlit");

    JzFileContextMenu::CreateList();
}