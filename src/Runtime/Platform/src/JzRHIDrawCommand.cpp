/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Platform/JzRHIDrawCommand.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"

JzRE::JzRHIDrawCommand::JzRHIDrawCommand(const JzRE::JzDrawParams &params) :
    JzRHICommand(JzRHIECommandType::Draw),
    m_params(params) { }

void JzRE::JzRHIDrawCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.Draw(m_params);
}