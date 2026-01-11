/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/UI/JzShaderContextMenu.h"
#include "JzRE/Editor/UI/JzMenuItem.h"

JzRE::JzShaderContextMenu::JzShaderContextMenu(const JzRE::String &filePath) :
    JzFileContextMenu(filePath) { }

void JzRE::JzShaderContextMenu::CreateList()
{
    JzFileContextMenu::CreateList();

    auto &compileAction         = CreateWidget<JzMenuItem>("Compile");
    compileAction.ClickedEvent += [this] {
        // TODO
    };
}