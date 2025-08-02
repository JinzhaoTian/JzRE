#pragma once

#include "CommonTypes.h"
#include "JzDevice.h"
#include "JzDeviceSettings.h"
#include "JzInputManager.h"
#include "JzRHICommandQueue.h"
#include "JzRHIDevice.h"
#include "JzRHIETypes.h"
#include "JzRHIFactory.h"
#include "JzSceneManager.h"
#include "JzServiceContainer.h"
#include "JzUIManager.h"
#include "JzWindow.h"
#include "JzWindowSettings.h"

namespace JzRE {
/**
 * @brief Context of JzRE
 */
class JzContext {
public:
    /**
     * @brief Construct a new Context object
     */
    JzContext(JzERHIType rhiType = JzERHIType::Unknown);

    JzContext(const JzContext &) = delete;

    JzContext &operator=(const JzContext &) = delete;

    /**
     * @brief Destroy the Context object
     */
    virtual ~JzContext();

    std::shared_ptr<JzRHIDevice> GetDevice() const;

    JzERHIType GetRHIType() const;

    std::shared_ptr<JzRHICommandQueue> GetCommandQueue() const;

    void SetThreadCount(U32 threadCount);

    U32 GetThreadCount() const;

public:
    std::unique_ptr<JzDevice>       device;
    std::unique_ptr<JzWindow>       window;
    std::unique_ptr<JzInputManager> inputManager;
    std::unique_ptr<JzUIManager>    uiManager;

    JzSceneManager sceneManager;

    JzDeviceSettings deviceSettings;
    JzWindowSettings windowSettings;

private:
    std::shared_ptr<JzRHIDevice>       m_device;
    std::shared_ptr<JzRHICommandQueue> m_commandQueue;
};
} // namespace JzRE
