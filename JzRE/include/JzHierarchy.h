#pragma once

#include "CommonTypes.h"
#include "JzGroup.h"
#include "JzPanelWindow.h"
#include "JzSeparator.h"
#include "JzTreeNode.h"
#include "JzWidgetContainer.h"

namespace JzRE {
class JzHierarchy : public JzPanelWindow {
public:
    JzHierarchy(const String &name, Bool is_opened);

    void Clear();

    void UnselectActorsWidgets();

    void SelectActorByWidget(JzTreeNode &widget);

public:
    JzEvent<String> SelectedEvent;
    JzEvent<String> UnselectedEvent;

private:
    std::unordered_map<String, JzTreeNode *> m_widgetActorLink;
    JzWidgetContainer                       &m_actions;
    JzWidgetContainer                       &m_actors;
};
} // namespace JzRE