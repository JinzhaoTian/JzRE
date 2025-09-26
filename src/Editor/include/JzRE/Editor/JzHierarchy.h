/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/UI/JzPanelWindow.h"
#include "JzRE/UI/JzTreeNode.h"
#include "JzRE/UI/JzWidgetContainer.h"

namespace JzRE {
/**
 * @brief Hierarchy Panel Window
 */
class JzHierarchy : public JzPanelWindow {
public:
    /**
     * @brief Constructor
     */
    JzHierarchy(const String &name, Bool is_opened);

    /**
     * @brief Clear Content
     */
    void Clear();

    /**
     * @brief Clear Select Status
     */
    void UnselectActorsWidgets();

    /**
     * @brief Select
     */
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