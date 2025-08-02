#include "JzRHICommandBuffer.h"

// RHICommandBuffer实现
JzRE::JzRHICommandBuffer::JzRHICommandBuffer(const JzRE::String &debugName) :
    m_debugName(debugName), m_isRecording(false)
{
}

JzRE::JzRHICommandBuffer::~JzRHICommandBuffer()
{
    Reset();
}

void JzRE::JzRHICommandBuffer::Begin()
{
    std::lock_guard<std::mutex> lock(m_commandMutex);
    if (m_isRecording) {
        std::cerr << "Command buffer is recording" << std::endl;
        return;
    }
    m_isRecording = true;
    Reset();
}

void JzRE::JzRHICommandBuffer::End()
{
    std::lock_guard<std::mutex> lock(m_commandMutex);
    if (!m_isRecording) {
        std::cerr << "Command buffer is not recording" << std::endl;
        return;
    }
    m_isRecording = false;
}

void JzRE::JzRHICommandBuffer::Reset()
{
    std::lock_guard<std::mutex> lock(m_commandMutex);
    m_commands.clear();
}

void JzRE::JzRHICommandBuffer::Execute()
{
    std::lock_guard<std::mutex> lock(m_commandMutex);
    if (m_isRecording) {
        std::cerr << "Cannot execute command buffer that is recording" << std::endl;
        return;
    }

    for (auto &command : m_commands) {
        command->Execute();
    }
}

JzRE::Bool JzRE::JzRHICommandBuffer::IsRecording() const
{
    return m_isRecording;
}

JzRE::Bool JzRE::JzRHICommandBuffer::IsEmpty() const
{
    return m_commands.empty();
}

JzRE::Size JzRE::JzRHICommandBuffer::GetCommandCount() const
{
    return m_commands.size();
}

const JzRE::String &JzRE::JzRHICommandBuffer::GetDebugName() const
{
    return m_debugName;
}

void JzRE::JzRHICommandBuffer::Clear(const JzRE::JzClearParams &params)
{
    AddCommand<JzRHIClearCommand>(params);
}

void JzRE::JzRHICommandBuffer::Draw(const JzRE::JzDrawParams &params)
{
    AddCommand<JzRHIDrawCommand>(params);
}

void JzRE::JzRHICommandBuffer::DrawIndexed(const JzRE::JzDrawIndexedParams &params)
{
    AddCommand<JzRHIDrawIndexedCommand>(params);
}

void JzRE::JzRHICommandBuffer::BindPipeline(std::shared_ptr<JzRE::JzRHIPipeline> pipeline)
{
    AddCommand<JzRHIBindPipelineCommand>(pipeline);
}

void JzRE::JzRHICommandBuffer::BindVertexArray(std::shared_ptr<JzRE::JzRHIVertexArray> vertexArray)
{
    AddCommand<JzRHIBindVertexArrayCommand>(vertexArray);
}

void JzRE::JzRHICommandBuffer::BindTexture(std::shared_ptr<JzRE::JzRHITexture> texture, U32 slot)
{
    AddCommand<JzRHIBindTextureCommand>(texture, slot);
}

void JzRE::JzRHICommandBuffer::SetViewport(const JzRE::JzViewport &viewport)
{
    AddCommand<JzRHISetViewportCommand>(viewport);
}

void JzRE::JzRHICommandBuffer::SetScissor(const JzRE::JzScissorRect &scissor)
{
    AddCommand<JzRHISetScissorCommand>(scissor);
}

void JzRE::JzRHICommandBuffer::BeginRenderPass(std::shared_ptr<JzRE::JzRHIFramebuffer> framebuffer)
{
    AddCommand<JzRHIBeginRenderPassCommand>(framebuffer);
}

void JzRE::JzRHICommandBuffer::EndRenderPass()
{
    AddCommand<JzRHIEndRenderPassCommand>();
}

template <typename T, typename... Args>
void JzRE::JzRHICommandBuffer::AddCommand(Args &&...args)
{
    if (!m_isRecording) {
        std::cerr << "Command buffer is not recording" << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(m_commandMutex);
    m_commands.push_back(std::make_unique<T>(std::forward<Args>(args)...));
}
