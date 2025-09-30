/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/RHI/JzRHICommand.h"
#include "JzRE/RHI/JzGPUFramebufferObject.h"

namespace JzRE {

/**
 * @brief Begin Render Pass Command
 */
class JzRHIBeginRenderPassCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param renderPass The render pass to begin
     */
    JzRHIBeginRenderPassCommand(std::shared_ptr<JzGPUFramebufferObject> framebuffer);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    std::shared_ptr<JzGPUFramebufferObject> m_framebuffer;
};

/**
 * @brief End Render Pass Command
 */
class JzRHIEndRenderPassCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     */
    JzRHIEndRenderPassCommand();

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    std::shared_ptr<JzGPUFramebufferObject> m_framebuffer;
};

} // namespace JzRE