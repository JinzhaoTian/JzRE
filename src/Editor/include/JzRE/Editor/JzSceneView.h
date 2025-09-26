/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzEGizmo.h"
#include "JzView.h"

namespace JzRE {
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

private:
    void HandleActorPicking();

private:
    JzEGizmoOperation m_currentOperation = JzEGizmoOperation::TRANSLATE;
};

} // namespace JzRE