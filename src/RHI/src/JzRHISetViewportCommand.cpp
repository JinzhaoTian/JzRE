/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/RHI/JzRHISetViewportCommand.h"
#include "JzRE/RHI/JzRHIDevice.h"
#include "JzRE/Core/JzServiceContainer.h"

JzRE::JzRHISetViewportCommand::JzRHISetViewportCommand(const JzRE::JzViewport &viewport) :
    JzRHICommand(JzRHIECommandType::SetViewport),
    m_viewport(viewport) { }

void JzRE::JzRHISetViewportCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzRHIDevice>();
    device.SetViewport(m_viewport);
}