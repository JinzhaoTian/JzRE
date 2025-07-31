#include "JzRHICommand.h"
#include "JzRHIContext.h"

// 具体命令实现
JzRE::JzRHIClearCommand::JzRHIClearCommand(const JzClearParams &params) :
    JzRHICommand(JzERHICommandType::Clear), params(params) { }

void JzRE::JzRHIClearCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->Clear(params);
    }
}

JzRE::JzRHIDrawCommand::JzRHIDrawCommand(const JzRE::JzDrawParams &params) :
    JzRHICommand(JzERHICommandType::Draw), params(params)
{
}

void JzRE::JzRHIDrawCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->Draw(params);
    }
}

JzRE::JzRHIDrawIndexedCommand::JzRHIDrawIndexedCommand(const JzRE::JzDrawIndexedParams &params) :
    JzRHICommand(JzERHICommandType::DrawIndexed), params(params)
{
}

void JzRE::JzRHIDrawIndexedCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->DrawIndexed(params);
    }
}

JzRE::JzRHIBindPipelineCommand::JzRHIBindPipelineCommand(std::shared_ptr<JzRE::JzRHIPipeline> pipeline) :
    JzRHICommand(JzERHICommandType::BindPipeline), pipeline(pipeline)
{
}

void JzRE::JzRHIBindPipelineCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->BindPipeline(pipeline);
    }
}

JzRE::JzRHIBindVertexArrayCommand::JzRHIBindVertexArrayCommand(std::shared_ptr<JzRE::JzRHIVertexArray> vertexArray) :
    JzRHICommand(JzERHICommandType::BindVertexArray), vertexArray(vertexArray)
{
}

void JzRE::JzRHIBindVertexArrayCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->BindVertexArray(vertexArray);
    }
}

JzRE::JzRHIBindTextureCommand::JzRHIBindTextureCommand(std::shared_ptr<JzRE::JzRHITexture> texture, U32 slot) :
    JzRHICommand(JzERHICommandType::BindTexture), texture(texture), slot(slot)
{
}

void JzRE::JzRHIBindTextureCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->BindTexture(texture, slot);
    }
}

JzRE::JzRHISetViewportCommand::JzRHISetViewportCommand(const JzRE::JzViewport &viewport) :
    JzRHICommand(JzERHICommandType::SetViewport), viewport(viewport)
{
}

void JzRE::JzRHISetViewportCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->SetViewport(viewport);
    }
}