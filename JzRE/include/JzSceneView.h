#pragma once

#include "CommonTypes.h"
#include "JzEGizmoOperation.h"
#include "JzEditorActions.h"
#include "JzSceneManager.h"
#include "JzViewControllable.h"

namespace JzRE {
/**
 * @brief Scene View
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

    JzScene *GetScene();

private:
    virtual void DrawFrame() override;

private:
    JzSceneManager   &m_sceneManager;
    JzEGizmoOperation m_currentOperation = JzEGizmoOperation::TRANSLATE;
};
} // namespace JzRE