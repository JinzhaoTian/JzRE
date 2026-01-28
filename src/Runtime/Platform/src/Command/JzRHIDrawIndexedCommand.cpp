/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Platform/Command/JzRHIDrawIndexedCommand.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"

JzRE::JzRHIDrawIndexedCommand::JzRHIDrawIndexedCommand(const JzRE::JzDrawIndexedParams &params) :
    JzRHICommand(JzRHIECommandType::DrawIndexed),
    m_params(params) { }

void JzRE::JzRHIDrawIndexedCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.DrawIndexed(m_params);
}