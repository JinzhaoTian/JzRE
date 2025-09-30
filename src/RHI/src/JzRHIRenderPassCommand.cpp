/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/RHI/JzRHIRenderPassCommand.h"

JzRE::JzRHIBeginRenderPassCommand::JzRHIBeginRenderPassCommand(std::shared_ptr<JzRE::JzRHIFramebuffer> framebuffer) :
    JzRHICommand(JzRHIECommandType::BeginRenderPass),
    m_framebuffer(framebuffer) { }

void JzRE::JzRHIBeginRenderPassCommand::Execute()
{
    // TODO
}

JzRE::JzRHIEndRenderPassCommand::JzRHIEndRenderPassCommand() :
    JzRHICommand(JzRHIECommandType::EndRenderPass) { }

void JzRE::JzRHIEndRenderPassCommand::Execute()
{
    // TODO
}