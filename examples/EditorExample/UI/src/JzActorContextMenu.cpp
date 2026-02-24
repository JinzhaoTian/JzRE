/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Editor/UI/JzActorContextMenu.h"

JzRE::JzActorContextMenu::JzActorContextMenu() { }

void JzRE::JzActorContextMenu::Execute(JzEPluginExecutionContext context)
{
    if (m_widgets.size() > 0)
        JzContextMenu::Execute(context);
}