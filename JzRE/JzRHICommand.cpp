#include "JzRHICommand.h"
#include "JzRHIContext.h"

JzRE::JzRHIClearCommand::JzRHIClearCommand(const JzClearParams &params) :
    JzRHICommand(JzRHIECommandType::Clear), m_params(params) { }

void JzRE::JzRHIClearCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->Clear(m_params);
    }
}

JzRE::JzRHIDrawCommand::JzRHIDrawCommand(const JzRE::JzDrawParams &params) :
    JzRHICommand(JzRHIECommandType::Draw), m_params(params) { }

void JzRE::JzRHIDrawCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->Draw(m_params);
    }
}

JzRE::JzRHIDrawIndexedCommand::JzRHIDrawIndexedCommand(const JzRE::JzDrawIndexedParams &params) :
    JzRHICommand(JzRHIECommandType::DrawIndexed), m_params(params) { }

void JzRE::JzRHIDrawIndexedCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->DrawIndexed(m_params);
    }
}

JzRE::JzRHIBindPipelineCommand::JzRHIBindPipelineCommand(std::shared_ptr<JzRE::JzRHIPipeline> pipeline) :
    JzRHICommand(JzRHIECommandType::BindPipeline), m_pipeline(pipeline) { }

void JzRE::JzRHIBindPipelineCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->BindPipeline(m_pipeline);
    }
}

JzRE::JzRHIBindVertexArrayCommand::JzRHIBindVertexArrayCommand(std::shared_ptr<JzRE::JzRHIVertexArray> vertexArray) :
    JzRHICommand(JzRHIECommandType::BindVertexArray), m_vertexArray(vertexArray) { }

void JzRE::JzRHIBindVertexArrayCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->BindVertexArray(m_vertexArray);
    }
}

JzRE::JzRHIBindTextureCommand::JzRHIBindTextureCommand(std::shared_ptr<JzRE::JzRHITexture> texture, U32 slot) :
    JzRHICommand(JzRHIECommandType::BindTexture), m_texture(texture), m_slot(slot) { }

void JzRE::JzRHIBindTextureCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->BindTexture(m_texture, m_slot);
    }
}

JzRE::JzRHISetViewportCommand::JzRHISetViewportCommand(const JzRE::JzViewport &viewport) :
    JzRHICommand(JzRHIECommandType::SetViewport), m_viewport(viewport) { }

void JzRE::JzRHISetViewportCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->SetViewport(m_viewport);
    }
}

JzRE::JzRHISetScissorCommand::JzRHISetScissorCommand(const JzRE::JzScissorRect &scissor) :
    JzRHICommand(JzRHIECommandType::SetScissor), m_scissorRect(scissor) { }

void JzRE::JzRHISetScissorCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->SetScissor(m_scissorRect);
    }
}

JzRE::JzRHIBeginRenderPassCommand::JzRHIBeginRenderPassCommand(std::shared_ptr<JzRE::JzRHIFramebuffer> framebuffer) :
    JzRHICommand(JzRHIECommandType::BeginRenderPass), m_framebuffer(framebuffer) { }

void JzRE::JzRHIBeginRenderPassCommand::Execute()
{
    // TODO
}

JzRE::JzRHIEndRenderPassCommand::JzRHIEndRenderPassCommand() :
    JzRHICommand(JzRHIECommandType::EndRenderPass) { }

void JzRE::JzRHIEndRenderPassCommand::Execute()
{
    // TODO
}

JzRE::JzRHIUpdateBufferCommand::JzRHIUpdateBufferCommand(std::shared_ptr<JzRE::JzRHIBuffer> buffer, const void *data, JzRE::U64 size) :
    JzRHICommand(JzRHIECommandType::UpdateBuffer), m_buffer(buffer), m_data(data), m_size(size) { }

void JzRE::JzRHIUpdateBufferCommand::Execute()
{
    // TODO
}