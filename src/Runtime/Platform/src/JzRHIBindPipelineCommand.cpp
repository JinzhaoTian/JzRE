/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Platform/JzRHIBindPipelineCommand.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"

JzRE::JzRHIBindPipelineCommand::JzRHIBindPipelineCommand(std::shared_ptr<JzRE::JzRHIPipeline> pipeline) :
    JzRHICommand(JzRHIECommandType::BindPipeline),
    m_pipeline(pipeline) { }

void JzRE::JzRHIBindPipelineCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.BindPipeline(m_pipeline);
}
