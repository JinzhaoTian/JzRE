/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/Command/JzRHICommand.h"

namespace JzRE {

/**
 * @brief Clear parameters
 */
struct JzClearParams {
    Bool clearColor   = true;
    Bool clearDepth   = true;
    Bool clearStencil = false;
    F32  colorR       = 0.0f;
    F32  colorG       = 0.0f;
    F32  colorB       = 0.0f;
    F32  colorA       = 1.0f;
    F32  depth        = 1.0f;
    U32  stencil      = 0;
};

/**
 * @brief Clear Command
 */
class JzRHIClearCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param params The parameters of the command
     */
    JzRHIClearCommand(const JzClearParams &params);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    JzClearParams m_params;
};

} // namespace JzRE