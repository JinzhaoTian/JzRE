#include "JzRHICommandList.h"

// RHICommandBuffer实现
JzRE::JzRHICommandList::JzRHICommandList(const JzRE::String &debugName) :
    m_debugName(debugName), m_isRecording(false)
{
}

JzRE::JzRHICommandList::~JzRHICommandList()
{
    Reset();
}

void JzRE::JzRHICommandList::Begin()
{
    std::lock_guard<std::mutex> lock(m_commandMutex);
    if (m_isRecording) {
        std::cerr << "Command buffer is recording" << std::endl;
        return;
    }
    m_isRecording = true;
    Reset();
}

void JzRE::JzRHICommandList::End()
{
    std::lock_guard<std::mutex> lock(m_commandMutex);
    if (!m_isRecording) {
        std::cerr << "Command buffer is not recording" << std::endl;
        return;
    }
    m_isRecording = false;
}

void JzRE::JzRHICommandList::Reset()
{
    std::lock_guard<std::mutex> lock(m_commandMutex);
    m_commands.clear();
}

void JzRE::JzRHICommandList::Execute()
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

JzRE::Bool JzRE::JzRHICommandList::IsRecording() const
{
    return m_isRecording;
}

JzRE::Bool JzRE::JzRHICommandList::IsEmpty() const
{
    return m_commands.empty();
}

JzRE::Size JzRE::JzRHICommandList::GetCommandCount() const
{
    return m_commands.size();
}

const JzRE::String &JzRE::JzRHICommandList::GetDebugName() const
{
    return m_debugName;
}

void JzRE::JzRHICommandList::Clear(const JzRE::JzClearParams &params)
{
    AddCommand<JzRHIClearCommand>(params);
}

void JzRE::JzRHICommandList::Draw(const JzRE::JzDrawParams &params)
{
    AddCommand<JzRHIDrawCommand>(params);
}

void JzRE::JzRHICommandList::DrawIndexed(const JzRE::JzDrawIndexedParams &params)
{
    AddCommand<JzRHIDrawIndexedCommand>(params);
}

void JzRE::JzRHICommandList::BindPipeline(std::shared_ptr<JzRE::JzRHIPipeline> pipeline)
{
    AddCommand<JzRHIBindPipelineCommand>(pipeline);
}

void JzRE::JzRHICommandList::BindVertexArray(std::shared_ptr<JzRE::JzRHIVertexArray> vertexArray)
{
    AddCommand<JzRHIBindVertexArrayCommand>(vertexArray);
}

void JzRE::JzRHICommandList::BindTexture(std::shared_ptr<JzRE::JzRHITexture> texture, U32 slot)
{
    AddCommand<JzRHIBindTextureCommand>(texture, slot);
}

void JzRE::JzRHICommandList::SetViewport(const JzRE::JzViewport &viewport)
{
    AddCommand<JzRHISetViewportCommand>(viewport);
}

void JzRE::JzRHICommandList::SetScissor(const JzRE::JzScissorRect &scissor)
{
    AddCommand<JzRHISetScissorCommand>(scissor);
}

void JzRE::JzRHICommandList::BeginRenderPass(std::shared_ptr<JzRE::JzRHIFramebuffer> framebuffer)
{
    AddCommand<JzRHIBeginRenderPassCommand>(framebuffer);
}

void JzRE::JzRHICommandList::EndRenderPass()
{
    AddCommand<JzRHIEndRenderPassCommand>();
}

template <typename T, typename... Args>
void JzRE::JzRHICommandList::AddCommand(Args &&...args)
{
    if (!m_isRecording) {
        std::cerr << "Command buffer is not recording" << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(m_commandMutex);
    m_commands.push_back(std::make_unique<T>(std::forward<Args>(args)...));
}
