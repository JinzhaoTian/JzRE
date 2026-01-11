/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Platform/JzRHISetViewportCommand.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"

JzRE::JzRHISetViewportCommand::JzRHISetViewportCommand(const JzRE::JzViewport &viewport) :
    JzRHICommand(JzRHIECommandType::SetViewport),
    m_viewport(viewport) { }

void JzRE::JzRHISetViewportCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.SetViewport(m_viewport);
}