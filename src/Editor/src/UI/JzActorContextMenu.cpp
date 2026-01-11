#include "JzRE/Editor/UI/JzActorContextMenu.h"

JzRE::JzActorContextMenu::JzActorContextMenu() { }

void JzRE::JzActorContextMenu::Execute(JzEPluginExecutionContext context)
{
    if (m_widgets.size() > 0)
        JzContextMenu::Execute(context);
}