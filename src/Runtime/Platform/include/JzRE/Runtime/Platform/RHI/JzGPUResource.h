/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {
/**
 * @brief Interface of GPU resource, represents GPU resource abstraction
 */
class JzGPUResource {
public:
    /**
     * @brief Constructor
     */
    JzGPUResource(const String &debugName = "") :
        debugName(debugName) { }

    /**
     * @brief Destructor
     */
    virtual ~JzGPUResource() = default;

    /**
     * @brief Get the debug name
     */
    const String &GetDebugName() const
    {
        return debugName;
    }

    /**
     * @brief Set the debug name
     */
    void SetDebugName(const String &name)
    {
        debugName = name;
    }

protected:
    String debugName;
};

} // namespace JzRE