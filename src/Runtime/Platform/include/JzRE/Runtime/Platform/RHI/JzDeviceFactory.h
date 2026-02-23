/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <vector>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"

namespace JzRE {

class JzIWindowBackend;

/**
 * @brief RHI Device Factory
 */
class JzDeviceFactory {
public:
    /**
     * @brief Create a Device object
     *
     * @param rhiType Graphics API Types
     * @param windowBackend Optional window backend pointer used by APIs that
     * require native window integration (e.g. Vulkan surface creation).
     * @return std::unique_ptr<JzDevice>
     */
    static std::unique_ptr<JzDevice> CreateDevice(JzERHIType rhiType, JzIWindowBackend *windowBackend = nullptr);

    /**
     * @brief Get the Supported RHI Types object
     *
     * @return std::vector<JzERHIType>
     */
    static std::vector<JzERHIType> GetSupportedRHITypes();

    /**
     * @brief Get the Default RHI Type object
     *
     * @return JzERHIType
     */
    static JzERHIType GetDefaultRHIType();

    /**
     * @brief Is supported RHI Type
     *
     * @param rhiType Graphics API Types
     * @return Bool
     */
    static Bool IsRHITypeSupported(JzERHIType rhiType);

    /**
     * @brief Get the RHI Type name
     *
     * @param rhiType Graphics API Types
     * @return String
     */
    static String GetRHITypeName(JzERHIType rhiType);

    /**
     * @brief Check Vulkan runtime availability in the current process.
     *
     * This validates loader availability and required instance extensions
     * reported by GLFW.
     */
    static Bool IsVulkanSupported();

private:
    JzDeviceFactory() = default;
};

} // namespace JzRE
