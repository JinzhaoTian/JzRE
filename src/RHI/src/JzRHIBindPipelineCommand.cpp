/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/RHI/JzRHIBindPipelineCommand.h"
#include "JzRE/RHI/JzDevice.h"
#include "JzRE/Core/JzServiceContainer.h"

JzRE::JzRHIBindPipelineCommand::JzRHIBindPipelineCommand(std::shared_ptr<JzRE::JzRHIPipeline> pipeline) :
    JzRHICommand(JzRHIECommandType::BindPipeline),
    m_pipeline(pipeline) { }

void JzRE::JzRHIBindPipelineCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.BindPipeline(m_pipeline);
}
