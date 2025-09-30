/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Core/JzServiceContainer.h"
#include "JzRE/RHI/JzRHIClearCommand.h"
#include "JzRE/RHI/JzDevice.h"

JzRE::JzRHIClearCommand::JzRHIClearCommand(const JzClearParams &params) :
    JzRHICommand(JzRHIECommandType::Clear),
    m_params(params) { }

void JzRE::JzRHIClearCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.Clear(m_params);
}