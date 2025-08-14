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
 * @brief JzRE Macro Definitions
 */
#define JzRE_CONTEXT() \
    JzRE::JzServiceContainer::Get<JzRE::JzContext>()
#define JzRE_DEVICE() \
    JzRE::JzServiceContainer::Get<JzRE::JzContext>().GetDevice()
#define JzRE_DEVICE_TYPE() \
    JzRE::JzServiceContainer::Get<JzRE::JzContext>().GetRHIType()

/**
 * @brief Context of JzRE
 */
class JzContext {
public:
    /**
     * @brief Construct a new Context object
     */
    JzContext(JzERHIType rhiType = JzERHIType::OpenGL);

    /**
     * @brief Delete copy constructor
     */
    JzContext(const JzContext &) = delete;

    /**
     * @brief Delete copy assignment operator
     */
    JzContext &operator=(const JzContext &) = delete;

    /**
     * @brief Destroy the Context object
     */
    virtual ~JzContext();

    /**
     * @brief Get the device
     * @return The device
     */
    std::shared_ptr<JzRHIDevice> GetDevice() const;

    /**
     * @brief Get the RHI type
     * @return The RHI type
     */
    JzERHIType GetRHIType() const;

    /**
     * @brief Get the command queue
     * @return The command queue
     */
    std::shared_ptr<JzRHICommandQueue> GetCommandQueue() const;

    /**
     * @brief Set the thread count
     * @param threadCount The thread count
     */
    void SetThreadCount(U32 threadCount);

    /**
     * @brief Get the thread count
     * @return The thread count
     */
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
