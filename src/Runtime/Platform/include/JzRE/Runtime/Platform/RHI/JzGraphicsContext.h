/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <memory>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Platform/Window/JzIWindowBackend.h"

namespace JzRE {

/**
 * @brief Graphics context that owns the device and bridges window + RHI lifecycle.
 *
 * This class is responsible for:
 * - Creating the RHI device after the window context is ready
 * - Managing context switching and presentation
 * - Exposing the device to higher-level systems
 */
class JzGraphicsContext {
public:
    /**
     * @brief Default constructor.
     */
    JzGraphicsContext() = default;

    /**
     * @brief Destructor.
     */
    ~JzGraphicsContext();

    /**
     * @brief Initialize the graphics context.
     *
     * @param windowBackend Window backend used to manage the native context.
     * @param rhiType RHI type to create the device with.
     */
    void Initialize(JzIWindowBackend &windowBackend, JzERHIType rhiType);

    /**
     * @brief Shutdown and release resources.
     */
    void Shutdown();

    /**
     * @brief Get the underlying RHI device.
     *
     * @return JzDevice& Reference to the device.
     */
    JzDevice &GetDevice();

    /**
     * @brief Get the underlying RHI device.
     *
     * @return const JzDevice& Reference to the device.
     */
    const JzDevice &GetDevice() const;

    /**
     * @brief Make the window context current on the calling thread.
     *
     * @param threadIndex Thread index (reserved for future multi-context support).
     */
    void MakeCurrentContext(U32 threadIndex = 0);

    /**
     * @brief Begin a frame on the device.
     */
    void BeginFrame();

    /**
     * @brief End a frame on the device.
     */
    void EndFrame();

    /**
     * @brief Present the current frame.
     *
     * This will swap window buffers and perform any required device sync.
     */
    void Present();

    /**
     * @brief Check whether a device has been created.
     *
     * @return Bool True if initialized.
     */
    Bool IsInitialized() const;

private:
    JzIWindowBackend         *m_windowBackend = nullptr;
    std::unique_ptr<JzDevice> m_device;
    JzERHIType                m_rhiType = JzERHIType::Unknown;
};

} // namespace JzRE
