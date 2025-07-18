#pragma once

#include "CommonTypes.h"
#include "JzDeviceSettings.h"

namespace JzRE {
/**
 * OS-based Device
 */
class JzDevice {
public:
    /**
     * @brief Constructor
     */
    JzDevice(const JzDeviceSettings &deviceSettings);

    /**
     * @brief Destructor
     */
    ~JzDevice();

    /**
     * @brief Is Vsync enabled
     */
    Bool IsVsync() const;

    /**
     * @brief Enable/Disable Vsync
     */
    void SetVsync(Bool value);

    /**
     * @brief Poll events
     */
    void PollEvents() const;

    /**
     * @brief Get elapsed time
     */
    F32 GetElapsedTime() const;

private:
    Bool m_isVsync = true;
    Bool m_isAlive = false;
};
} // namespace JzRE
