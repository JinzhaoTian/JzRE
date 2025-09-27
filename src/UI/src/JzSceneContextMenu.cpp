/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/UI/JzSceneContextMenu.h"
#include "JzRE/UI/JzMenuItem.h"

JzRE::JzSceneContextMenu::JzSceneContextMenu(const String &filePath) :
    JzFileContextMenu(filePath) { }

void JzRE::JzSceneContextMenu::CreateList()
{
    JzFileContextMenu::CreateList();

    auto &editAction         = CreateWidget<JzMenuItem>("Edit");
    editAction.ClickedEvent += [this] {
        // TODO
    };
}