/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Editor/Core/JzEvent.h"

namespace JzRE {

/**
 * @brief Editor-level state for selection and editing operations.
 *
 * Provides centralized selection state that can be shared between editor panels.
 * Registered via JzServiceContainer for global access.
 */
struct JzEditorState {
    /**
     * @brief Currently selected entity in the editor
     */
    JzEntity selectedEntity = INVALID_ENTITY;

    /**
     * @brief Event fired when an entity is selected
     */
    JzEvent<JzEntity> EntitySelectedEvent;

    /**
     * @brief Event fired when selection is cleared
     */
    JzEvent<> SelectionClearedEvent;

    /**
     * @brief Select an entity and fire the selection event
     * @param entity The entity to select
     */
    void SelectEntity(JzEntity entity)
    {
        selectedEntity = entity;
        EntitySelectedEvent.Invoke(entity);
    }

    /**
     * @brief Clear the current selection and fire the cleared event
     */
    void ClearSelection()
    {
        selectedEntity = INVALID_ENTITY;
        SelectionClearedEvent.Invoke();
    }

    /**
     * @brief Check if there is a valid selection
     * @return True if an entity is selected
     */
    Bool HasSelection() const
    {
        return selectedEntity != INVALID_ENTITY;
    }
};

} // namespace JzRE
