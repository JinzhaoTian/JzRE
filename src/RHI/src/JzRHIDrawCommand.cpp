/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/RHI/JzRHIDrawCommand.h"
#include "JzRE/RHI/JzRHIDevice.h"
#include "JzRE/Core/JzServiceContainer.h"

JzRE::JzRHIDrawCommand::JzRHIDrawCommand(const JzRE::JzDrawParams &params) :
    JzRHICommand(JzRHIECommandType::Draw),
    m_params(params) { }

void JzRE::JzRHIDrawCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzRHIDevice>();
    device.Draw(m_params);
}