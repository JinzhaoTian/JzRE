/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Platform/JzRHISetScissorCommand.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"

JzRE::JzRHISetScissorCommand::JzRHISetScissorCommand(const JzRE::JzScissorRect &scissor) :
    JzRHICommand(JzRHIECommandType::SetScissor),
    m_scissorRect(scissor) { }

void JzRE::JzRHISetScissorCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.SetScissor(m_scissorRect);
}
