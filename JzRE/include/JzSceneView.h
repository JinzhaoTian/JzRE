#pragma once

#include "CommonTypes.h"
#include "JzEGizmo.h"
#include "JzEditorActions.h"
#include "JzSceneManager.h"
#include "JzViewControllable.h"

namespace JzRE {
/**
 * @brief Scene View Panel Window
 */
class JzSceneView : public JzViewControllable {
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
    virtual void Update(F32 deltaTime) override;

    /**
     * @brief Initialize the frame
     */
    virtual void InitFrame() override;

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
     * @brief Get the scene
     *
     * @return The scene
     */
    JzScene *GetScene() override;

private:
    virtual void DrawFrame() override;
    void         HandleActorPicking();

private:
    JzSceneManager   &m_sceneManager;
    JzEGizmoOperation m_currentOperation = JzEGizmoOperation::TRANSLATE;
};
} // namespace JzRE