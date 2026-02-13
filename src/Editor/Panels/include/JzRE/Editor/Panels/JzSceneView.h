/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzView.h"

namespace JzRE {

/**
 * @brief Enums of Gizmo Operations
 */
enum class JzEGizmoOperation : U8 {
    TRANSLATE,
    ROTATE,
    SCALE
};

/**
 * @brief Scene View Panel Window
 */
class JzSceneView : public JzView {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param is_opened
     */
    JzSceneView(const String &name, Bool is_opened);

    /**
     * @brief Update the scene view
     *
     * @param deltaTime
     */
    void Update(F32 deltaTime) override;

    /**
     * @brief Set the gizmo operation
     */
    void SetGizmoOperation(JzEGizmoOperation operation);

    /**
     * @brief Get the gizmo operation
     *
     * @return The gizmo operation
     */
    JzEGizmoOperation GetGizmoOperation() const;

    /**
     * @brief Set the currently selected entity for gizmo display
     * @param entity The entity to select
     */
    void SetSelectedEntity(JzEntity entity);

    /**
     * @brief Get the currently selected entity
     * @return The selected entity
     */
    JzEntity GetSelectedEntity() const;

protected:
    /**
     * @brief Get the camera entity for scene view rendering.
     *
     * Uses the main camera with orbit controller.
     *
     * @return The camera entity
     */
    JzEntity GetCameraEntity() override;

    /**
     * @brief SceneView renders game objects and editor objects.
     */
    JzRenderVisibility GetVisibility() const override
    {
        return JzRenderVisibility::MainScene | JzRenderVisibility::Overlay;
    }

    /**
     * @brief SceneView enables editor features by default (skybox, grid, axis).
     */
    JzRenderTargetFeatures GetRenderFeatures() const override;

private:
    void HandleActorPicking();

    /**
     * @brief Ensure camera entity has JzCameraInputComponent
     */
    void EnsureCameraInputComponent();

    /**
     * @brief Find and cache the editor camera entity
     */
    void FindEditorCamera();

    /**
     * @brief Create dedicated input state for scene view (isolated from asset view)
     */
    void CreateSceneInputState();

    /**
     * @brief Update dedicated input state from primary window input
     */
    void UpdateSceneInputState();

private:
    JzEGizmoOperation m_currentOperation = JzEGizmoOperation::TRANSLATE;
    JzEntity          m_editorCamera     = INVALID_ENTITY;

    // Dedicated input state for scene view (isolated from asset view)
    JzEntity m_sceneInputState = INVALID_ENTITY;

    // Mouse interaction state
    Bool     m_leftMousePressed  = false;
    Bool     m_rightMousePressed = false;
    JzVec2   m_lastMousePos      = JzVec2(0.0f, 0.0f);
    Bool     m_firstMouse        = true;
    Bool     m_cameraInitialized = false;
    JzIVec2  m_lastViewSize      = JzIVec2(0, 0);
    JzEntity m_selectedEntity    = INVALID_ENTITY;
};

} // namespace JzRE
