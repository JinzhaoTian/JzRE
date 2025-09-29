/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/RHI/JzRHIImGuiRenderCommand.h"
#include "JzRE/Core/JzServiceContainer.h"
#include "JzRE/RHI/JzRHIDevice.h"

JzRE::JzRHIImGuiRenderCommand::JzRHIImGuiRenderCommand(const JzImGuiRenderParams &params) :
    JzRHICommand(JzRHIECommandType::ImGuiRender),
    m_params(params) { }

void JzRE::JzRHIImGuiRenderCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzRHIDevice>();
    // TODO
}
