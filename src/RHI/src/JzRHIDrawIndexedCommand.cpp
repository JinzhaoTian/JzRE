/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/RHI/JzRHIDrawIndexedCommand.h"
#include "JzRE/RHI/JzDevice.h"
#include "JzRE/Core/JzServiceContainer.h"

JzRE::JzRHIDrawIndexedCommand::JzRHIDrawIndexedCommand(const JzRE::JzDrawIndexedParams &params) :
    JzRHICommand(JzRHIECommandType::DrawIndexed),
    m_params(params) { }

void JzRE::JzRHIDrawIndexedCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.DrawIndexed(m_params);
}