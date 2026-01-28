/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Platform/Command/JzRHIImGuiRenderCommand.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"

JzRE::JzRHIImGuiRenderCommand::JzRHIImGuiRenderCommand(const JzImGuiRenderParams &params) :
    JzRHICommand(JzRHIECommandType::ImGuiRender),
    m_params(params) { }

void JzRE::JzRHIImGuiRenderCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    // TODO
}
