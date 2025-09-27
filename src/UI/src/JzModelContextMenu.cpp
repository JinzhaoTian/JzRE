/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/UI/JzModelContextMenu.h"
#include "JzRE/UI/JzMenuItem.h"
#include "JzRE/UI/JzMenuList.h"

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