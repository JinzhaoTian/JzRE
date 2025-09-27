/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/UI/JzMaterialContextMenu.h"
#include "JzRE/UI/JzMenuItem.h"

JzRE::JzMaterialContextMenu::JzMaterialContextMenu(const JzRE::String &filePath) :
    JzFileContextMenu(filePath) { }

void JzRE::JzMaterialContextMenu::CreateList()
{
    JzFileContextMenu::CreateList();

    auto &editAction         = CreateWidget<JzMenuItem>("Edit");
    editAction.ClickedEvent += [this] {
        // TODO
    };

    auto &reload         = CreateWidget<JzMenuItem>("Reload");
    reload.ClickedEvent += [this] {
        // TODO
    };
}