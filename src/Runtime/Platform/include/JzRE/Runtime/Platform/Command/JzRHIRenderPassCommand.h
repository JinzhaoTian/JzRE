/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Platform/Command/JzRHICommand.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUFramebufferObject.h"
#include "JzRE/Runtime/Platform/RHI/JzRHIRenderPass.h"

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
    JzRHIBeginRenderPassCommand(std::shared_ptr<JzGPUFramebufferObject> framebuffer,
                                std::shared_ptr<JzRHIRenderPass>        renderPass = nullptr);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    std::shared_ptr<JzGPUFramebufferObject> m_framebuffer;
    std::shared_ptr<JzRHIRenderPass>        m_renderPass;
};

/**
 * @brief End Render Pass Command
 */
class JzRHIEndRenderPassCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     */
    JzRHIEndRenderPassCommand(std::shared_ptr<JzRHIRenderPass> renderPass = nullptr);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    std::shared_ptr<JzGPUFramebufferObject> m_framebuffer;
    std::shared_ptr<JzRHIRenderPass>        m_renderPass;
};

} // namespace JzRE
