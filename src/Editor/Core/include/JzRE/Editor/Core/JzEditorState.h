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
     * @brief Whether SceneView skybox rendering is enabled.
     */
    Bool sceneSkyboxEnabled = true;

    /**
     * @brief Whether SceneView axis rendering is enabled.
     */
    Bool sceneAxisEnabled = true;

    /**
     * @brief Whether SceneView ground grid rendering is enabled.
     */
    Bool sceneGridEnabled = true;

    /**
     * @brief Event fired when SceneView skybox enabled state changes.
     */
    JzEvent<Bool> SceneSkyboxToggledEvent;

    /**
     * @brief Event fired when SceneView axis enabled state changes.
     */
    JzEvent<Bool> SceneAxisToggledEvent;

    /**
     * @brief Event fired when SceneView grid enabled state changes.
     */
    JzEvent<Bool> SceneGridToggledEvent;

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

    /**
     * @brief Enable or disable SceneView skybox rendering.
     *
     * @param enabled True to enable skybox, false to disable.
     */
    void SetSceneSkyboxEnabled(Bool enabled)
    {
        if (sceneSkyboxEnabled == enabled) {
            return;
        }

        sceneSkyboxEnabled = enabled;
        SceneSkyboxToggledEvent.Invoke(enabled);
    }

    /**
     * @brief Enable or disable SceneView axis rendering.
     *
     * @param enabled True to enable axis, false to disable.
     */
    void SetSceneAxisEnabled(Bool enabled)
    {
        if (sceneAxisEnabled == enabled) {
            return;
        }

        sceneAxisEnabled = enabled;
        SceneAxisToggledEvent.Invoke(enabled);
    }

    /**
     * @brief Enable or disable SceneView ground grid rendering.
     *
     * @param enabled True to enable grid, false to disable.
     */
    void SetSceneGridEnabled(Bool enabled)
    {
        if (sceneGridEnabled == enabled) {
            return;
        }

        sceneGridEnabled = enabled;
        SceneGridToggledEvent.Invoke(enabled);
    }
};

} // namespace JzRE
