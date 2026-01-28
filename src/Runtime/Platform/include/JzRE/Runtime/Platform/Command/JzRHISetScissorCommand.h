/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/Command/JzRHICommand.h"

namespace JzRE {

/**
 * @brief Scissor rectangle description
 */
struct JzScissorRect {
    I32 x      = 0;
    I32 y      = 0;
    U32 width  = 800;
    U32 height = 600;
};

/**
 * @brief Set Scissor Command
 */
class JzRHISetScissorCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param scissor The scissor to set
     */
    JzRHISetScissorCommand(const JzScissorRect &scissor);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    JzScissorRect m_scissorRect;
};

} // namespace JzRE