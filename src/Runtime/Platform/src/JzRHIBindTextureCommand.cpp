/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Platform/JzRHIBindTextureCommand.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"

JzRE::JzRHIBindTextureCommand::JzRHIBindTextureCommand(std::shared_ptr<JzRE::JzGPUTextureObject> texture, U32 slot) :
    JzRHICommand(JzRHIECommandType::BindTexture),
    m_texture(texture),
    m_slot(slot) { }

void JzRE::JzRHIBindTextureCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.BindTexture(m_texture, m_slot);
}