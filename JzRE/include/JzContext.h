#pragma once

#include "CommonTypes.h"
#include "JzDevice.h"
#include "JzDeviceSettings.h"
#include "JzInputManager.h"
#include "JzSceneManager.h"
#include "JzUIManager.h"
#include "JzWindow.h"
#include "JzWindowSettings.h"

namespace JzRE {
/**
 * @brief Context of the render engine
 */
class JzContext {
public:
    /**
     * @brief Construct a new Context object
     *
     * @param projectPath
     * @param projectName
     */
    JzContext();

    /**
     * @brief Destroy the Context object
     */
    ~JzContext();

public:
    std::unique_ptr<JzDevice>       device;
    std::unique_ptr<JzWindow>       window;
    std::unique_ptr<JzInputManager> inputManager;
    std::unique_ptr<JzUIManager>    uiManager;

    JzSceneManager sceneManager;

    JzWindowSettings windowSettings;
};
} // namespace JzRE
