/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzHierarchy.h"
#include "JzRE/UI/JzTreeNode.h"
#include "JzRE/UI/JzGroup.h"
#include "JzRE/UI/JzSeparator.h"

JzRE::JzHierarchy::JzHierarchy(const JzRE::String &name, JzRE::Bool is_opened) :
    JzPanelWindow(name, is_opened),
    m_actions(CreateWidget<JzGroup>()),
    m_actors(CreateWidget<JzGroup>())
{
    CreateWidget<JzSeparator>();
    CreateWidget<JzSeparator>();
    CreateWidget<JzSeparator>();
}

void JzRE::JzHierarchy::Clear()
{
    // EDITOR_EXEC(UnselectActor());

    m_actors.RemoveAllWidgets();
    m_widgetActorLink.clear();
}

void JzRE::JzHierarchy::UnselectActorsWidgets()
{
    for (auto &widget : m_widgetActorLink)
        widget.second->selected = false;
}

void ExpandTreeNode(JzRE::JzTreeNode &node)
{
    node.Open();

    if (node.HasParent()) {
        if (auto parent = dynamic_cast<JzRE::JzTreeNode *>(node.GetParent()); parent) {
            ExpandTreeNode(*parent);
        }
    }
}

void JzRE::JzHierarchy::SelectActorByWidget(JzRE::JzTreeNode &widget)
{
    UnselectActorsWidgets();

    widget.selected = true;

    if (widget.HasParent()) {
        if (auto parent = dynamic_cast<JzTreeNode *>(widget.GetParent()); parent) {
            ExpandTreeNode(*parent);
        }
    }
}
