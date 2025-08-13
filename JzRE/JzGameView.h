#pragma once

#include "CommonTypes.h"
#include "JzCamera.h"
#include "JzEditorActions.h"
#include "JzScene.h"
#include "JzSceneManager.h"
#include "JzView.h"

namespace JzRE {
/**
 * @brief Game View Panel Window
 */
class JzGameView : public JzView {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param is_opened
     */
    JzGameView(const String &name, Bool is_opened);

    /**
     * @brief Get the camera
     *
     * @return The camera
     */
    JzCamera *GetCamera();

    /**
     * @brief Get the scene
     *
     * @return The scene
     */
    JzScene *GetScene();

private:
    JzSceneManager &m_sceneManager;
};
} // namespace JzRE