/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/RHI/JzRHIBindTextureCommand.h"
#include "JzRE/RHI/JzRHIDevice.h"
#include "JzRE/Core/JzServiceContainer.h"

JzRE::JzRHIBindTextureCommand::JzRHIBindTextureCommand(std::shared_ptr<JzRE::JzRHITexture> texture, U32 slot) :
    JzRHICommand(JzRHIECommandType::BindTexture),
    m_texture(texture),
    m_slot(slot) { }

void JzRE::JzRHIBindTextureCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzRHIDevice>();
    device.BindTexture(m_texture, m_slot);
}