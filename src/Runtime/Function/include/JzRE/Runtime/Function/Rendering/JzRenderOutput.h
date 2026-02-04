/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"

namespace JzRE {

/**
 * @brief UI-agnostic render output interface.
 *
 * This abstraction allows UI frameworks to consume render outputs
 * without depending on concrete render target implementations.
 */
class JzRenderOutput {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~JzRenderOutput() = default;

    /**
     * @brief Get the native texture ID for UI consumption.
     *
     * @return The native texture ID (e.g., GLuint cast to void*)
     */
    virtual void *GetTextureID() const = 0;

    /**
     * @brief Get current output size.
     *
     * @return The output size
     */
    virtual JzIVec2 GetSize() const = 0;

    /**
     * @brief Check if the output is valid and ready for use.
     *
     * @return true if the output is valid
     */
    virtual Bool IsValid() const = 0;
};

} // namespace JzRE
