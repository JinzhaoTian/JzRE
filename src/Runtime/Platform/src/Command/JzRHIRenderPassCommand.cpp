/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Platform/Command/JzRHIRenderPassCommand.h"

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"

JzRE::JzRHIBeginRenderPassCommand::JzRHIBeginRenderPassCommand(
    std::shared_ptr<JzRE::JzGPUFramebufferObject> framebuffer,
    std::shared_ptr<JzRE::JzRHIRenderPass>        renderPass) :
    JzRHICommand(JzRHIECommandType::BeginRenderPass),
    m_framebuffer(std::move(framebuffer)),
    m_renderPass(std::move(renderPass)) { }

void JzRE::JzRHIBeginRenderPassCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.BindFramebuffer(m_framebuffer);
    if (m_renderPass) {
        m_renderPass->OnBegin(device, m_framebuffer);
    }
}

JzRE::JzRHIEndRenderPassCommand::JzRHIEndRenderPassCommand(
    std::shared_ptr<JzRE::JzRHIRenderPass> renderPass) :
    JzRHICommand(JzRHIECommandType::EndRenderPass),
    m_renderPass(std::move(renderPass)) { }

void JzRE::JzRHIEndRenderPassCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    if (m_renderPass) {
        m_renderPass->OnEnd(device);
    }
    device.BindFramebuffer(nullptr);
}
