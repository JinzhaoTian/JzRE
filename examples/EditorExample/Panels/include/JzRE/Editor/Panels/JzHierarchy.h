/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Editor/UI/JzPanelWindow.h"
#include "JzRE/Editor/UI/JzTreeNode.h"
#include "JzRE/Editor/UI/JzWidgetContainer.h"

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
     * @brief Update the hierarchy panel each frame
     * @param deltaTime Time since last frame in seconds
     */
    void Update(F32 deltaTime);

    /**
     * @brief Refresh the entity list from JzWorld
     */
    void RefreshEntityList();

    /**
     * @brief Clear Content
     */
    void Clear();

    /**
     * @brief Clear Select Status
     */
    void UnselectActorsWidgets();

    /**
     * @brief Select entity by widget
     */
    void SelectActorByWidget(JzTreeNode &widget);

    /**
     * @brief Select entity by JzEntity handle
     * @param entity The entity to select
     */
    void SelectEntity(JzEntity entity);

    /**
     * @brief Create an empty entity with default components
     */
    void CreateEmptyEntity();

    /**
     * @brief Delete the currently selected entity
     */
    void DeleteSelectedEntity();

    /**
     * @brief Open file dialog and add a model to the scene
     */
    void AddModelFromFile();

    /**
     * @brief Mark the hierarchy as needing a refresh
     */
    void MarkDirty()
    {
        m_needsRefresh = true;
    }

public:
    JzEvent<JzEntity> EntitySelectedEvent;
    JzEvent<>         SelectionClearedEvent;

private:
    std::unordered_map<JzEntity, JzTreeNode *> m_entityWidgetMap;
    JzWidgetContainer                         &m_actions;
    JzWidgetContainer                         &m_actors;
    Bool                                       m_needsRefresh  = true;
    U32                                        m_entityCounter = 0;
};
} // namespace JzRE