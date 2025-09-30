/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/RHI/JzRHIBindPipelineCommand.h"
#include "JzRE/RHI/JzRHIDevice.h"
#include "JzRE/Core/JzServiceContainer.h"

JzRE::JzRHIBindPipelineCommand::JzRHIBindPipelineCommand(std::shared_ptr<JzRE::JzRHIPipeline> pipeline) :
    JzRHICommand(JzRHIECommandType::BindPipeline),
    m_pipeline(pipeline) { }

void JzRE::JzRHIBindPipelineCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzRHIDevice>();
    device.BindPipeline(m_pipeline);
}
