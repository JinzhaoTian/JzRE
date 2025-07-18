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
    JzContext(const String &projectPath, const String &projectName);

    /**
     * @brief Destroy the Context object
     */
    ~JzContext();

public:
    const String projectPath;
    const String projectName;

    UniquePtr<JzDevice>       device;
    UniquePtr<JzWindow>       window;
    UniquePtr<JzInputManager> inputManager;
    UniquePtr<JzUIManager>    uiManager;

    JzSceneManager sceneManager;

    JzWindowSettings windowSettings;
};
} // namespace JzRE
