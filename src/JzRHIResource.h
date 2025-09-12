#pragma once

#include "JzRETypes.h"
#include "JzRHIETypes.h"

namespace JzRE {
/**
 * @brief Interface for RHI Resource
 */
class JzRHIResource {
public:
    /**
     * @brief Constructor
     */
    JzRHIResource(const String &debugName = "") :
        debugName(debugName) { }

    /**
     * @brief Destructor
     */
    virtual ~JzRHIResource() = default;

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