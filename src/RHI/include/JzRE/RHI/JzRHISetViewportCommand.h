/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/RHI/JzRHICommand.h"

namespace JzRE {

/**
 * @brief Viewport description
 */
struct JzViewport {
    F32 x        = 0.0f;
    F32 y        = 0.0f;
    F32 width    = 800.0f;
    F32 height   = 600.0f;
    F32 minDepth = 0.0f;
    F32 maxDepth = 1.0f;
};

/**
 * @brief Set Viewport Command
 */
class JzRHISetViewportCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param viewport The viewport to set
     */
    JzRHISetViewportCommand(const JzViewport &viewport);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    JzViewport m_viewport;
};

} // namespace JzRE