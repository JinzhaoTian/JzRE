/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Platform/Command/JzRHICommandList.h"

#include "JzRE/Runtime/Core/JzLogger.h"

JzRE::JzRHICommandList::JzRHICommandList(const JzRE::String &debugName) :
    m_debugName(debugName),
    m_isRecording(false)
{ }

JzRE::JzRHICommandList::~JzRHICommandList()
{
    Reset();
}

void JzRE::JzRHICommandList::Begin()
{
    std::lock_guard<std::mutex> lock(m_commandMutex);
    if (m_isRecording) {
        JzRE_LOG_ERROR("Command buffer is recording");
        return;
    }

    // Avoid recursive lock from calling Reset() under the same mutex.
    m_commands.clear();
    m_isRecording = true;
}

void JzRE::JzRHICommandList::End()
{
    std::lock_guard<std::mutex> lock(m_commandMutex);
    if (!m_isRecording) {
        JzRE_LOG_ERROR("Command buffer is not recording");
        return;
    }
    m_isRecording = false;
}

void JzRE::JzRHICommandList::Reset()
{
    std::lock_guard<std::mutex> lock(m_commandMutex);
    m_isRecording = false;
    m_commands.clear();
}

std::vector<JzRE::JzRHIRecordedCommand> JzRE::JzRHICommandList::GetCommands() const
{
    std::lock_guard<std::mutex> lock(m_commandMutex);
    if (m_isRecording) {
        JzRE_LOG_WARN("Command buffer '{}' is still recording; returning current snapshot", m_debugName);
    }
    return m_commands;
}

JzRE::Bool JzRE::JzRHICommandList::IsEmpty() const
{
    std::lock_guard<std::mutex> lock(m_commandMutex);
    return m_commands.empty();
}

JzRE::Size JzRE::JzRHICommandList::GetCommandCount() const
{
    std::lock_guard<std::mutex> lock(m_commandMutex);
    return m_commands.size();
}

const JzRE::String &JzRE::JzRHICommandList::GetDebugName() const
{
    return m_debugName;
}

JzRE::Bool JzRE::JzRHICommandList::IsRecording() const
{
    std::lock_guard<std::mutex> lock(m_commandMutex);
    return m_isRecording;
}

void JzRE::JzRHICommandList::Clear(const JzRE::JzClearParams &params)
{
    AddCommand(JzRHIECommandType::Clear, params);
}

void JzRE::JzRHICommandList::Draw(const JzRE::JzDrawParams &params)
{
    AddCommand(JzRHIECommandType::Draw, params);
}

void JzRE::JzRHICommandList::DrawIndexed(const JzRE::JzDrawIndexedParams &params)
{
    AddCommand(JzRHIECommandType::DrawIndexed, params);
}

void JzRE::JzRHICommandList::BindPipeline(std::shared_ptr<JzRE::JzRHIPipeline> pipeline)
{
    JzRHIBindPipelinePayload payload;
    payload.pipeline = std::move(pipeline);
    AddCommand(JzRHIECommandType::BindPipeline, std::move(payload));
}

void JzRE::JzRHICommandList::BindVertexArray(std::shared_ptr<JzRE::JzGPUVertexArrayObject> vertexArray)
{
    JzRHIBindVertexArrayPayload payload;
    payload.vertexArray = std::move(vertexArray);
    AddCommand(JzRHIECommandType::BindVertexArray, std::move(payload));
}

void JzRE::JzRHICommandList::BindTexture(std::shared_ptr<JzRE::JzGPUTextureObject> texture, U32 slot)
{
    JzRHIBindTexturePayload payload;
    payload.texture = std::move(texture);
    payload.slot    = slot;
    AddCommand(JzRHIECommandType::BindTexture, std::move(payload));
}

void JzRE::JzRHICommandList::BindFramebuffer(std::shared_ptr<JzRE::JzGPUFramebufferObject> framebuffer)
{
    JzRHIBindFramebufferPayload payload;
    payload.framebuffer = std::move(framebuffer);
    AddCommand(JzRHIECommandType::BindFramebuffer, std::move(payload));
}

void JzRE::JzRHICommandList::SetViewport(const JzRE::JzViewport &viewport)
{
    AddCommand(JzRHIECommandType::SetViewport, viewport);
}

void JzRE::JzRHICommandList::SetScissor(const JzRE::JzScissorRect &scissor)
{
    AddCommand(JzRHIECommandType::SetScissor, scissor);
}

void JzRE::JzRHICommandList::ResourceBarrier(const std::vector<JzRE::JzRHIResourceBarrier> &barriers)
{
    if (barriers.empty()) {
        return;
    }

    JzRHIResourceBarrierPayload payload;
    payload.barriers = barriers;
    AddCommand(JzRHIECommandType::ResourceBarrier, std::move(payload));
}

void JzRE::JzRHICommandList::BlitFramebufferToScreen(
    std::shared_ptr<JzRE::JzGPUFramebufferObject> framebuffer,
    U32 srcWidth, U32 srcHeight,
    U32 dstWidth, U32 dstHeight)
{
    JzRHIBlitFramebufferToScreenPayload payload;
    payload.framebuffer = std::move(framebuffer);
    payload.srcWidth    = srcWidth;
    payload.srcHeight   = srcHeight;
    payload.dstWidth    = dstWidth;
    payload.dstHeight   = dstHeight;
    AddCommand(JzRHIECommandType::BlitFramebufferToScreen, std::move(payload));
}

void JzRE::JzRHICommandList::BeginRenderPass(std::shared_ptr<JzRE::JzGPUFramebufferObject> framebuffer)
{
    BeginRenderPass(nullptr, std::move(framebuffer));
}

void JzRE::JzRHICommandList::BeginRenderPass(std::shared_ptr<JzRE::JzRHIRenderPass>        renderPass,
                                             std::shared_ptr<JzRE::JzGPUFramebufferObject> framebuffer)
{
    JzRHIBeginRenderPassPayload payload;
    payload.framebuffer = std::move(framebuffer);
    payload.renderPass  = std::move(renderPass);
    AddCommand(JzRHIECommandType::BeginRenderPass, std::move(payload));
}

void JzRE::JzRHICommandList::EndRenderPass()
{
    EndRenderPass(nullptr);
}

void JzRE::JzRHICommandList::EndRenderPass(std::shared_ptr<JzRE::JzRHIRenderPass> renderPass)
{
    JzRHIEndRenderPassPayload payload;
    payload.renderPass = std::move(renderPass);
    AddCommand(JzRHIECommandType::EndRenderPass, std::move(payload));
}

template <typename TPayload>
void JzRE::JzRHICommandList::AddCommand(JzRE::JzRHIECommandType type, TPayload &&payload)
{
    std::lock_guard<std::mutex> lock(m_commandMutex);
    if (!m_isRecording) {
        JzRE_LOG_ERROR("Command buffer '{}' is not recording", m_debugName);
        return;
    }

    JzRHIRecordedCommand command;
    command.type    = type;
    command.payload = std::forward<TPayload>(payload);
    m_commands.push_back(std::move(command));
}
