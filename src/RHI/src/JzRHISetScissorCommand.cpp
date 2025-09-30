/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/RHI/JzRHISetScissorCommand.h"
#include "JzRE/RHI/JzRHIDevice.h"
#include "JzRE/Core/JzServiceContainer.h"

JzRE::JzRHISetScissorCommand::JzRHISetScissorCommand(const JzRE::JzScissorRect &scissor) :
    JzRHICommand(JzRHIECommandType::SetScissor),
    m_scissorRect(scissor) { }

void JzRE::JzRHISetScissorCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzRHIDevice>();
    device.SetScissor(m_scissorRect);
}
