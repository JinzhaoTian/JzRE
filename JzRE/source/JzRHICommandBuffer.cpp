#include "JzRHICommandBuffer.h"

// RHICommandBuffer实现
JzRE::JzRHICommandBuffer::JzRHICommandBuffer(const JzRE::String &debugName) :
    debugName(debugName)
{
}

JzRE::JzRHICommandBuffer::~JzRHICommandBuffer()
{
    Reset();
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
    // TODO: 实现剪裁矩形命令
}

void JzRE::JzRHICommandBuffer::BeginRenderPass(std::shared_ptr<JzRE::JzRHIFramebuffer> framebuffer)
{
    // TODO: 实现渲染通道开始命令
}

void JzRE::JzRHICommandBuffer::EndRenderPass()
{
    // TODO: 实现渲染通道结束命令
}

void JzRE::JzRHICommandBuffer::Begin()
{
    std::lock_guard<std::mutex> lock(commandMutex);
    if (isRecording) {
        std::cerr << "命令缓冲已经在记录中" << std::endl;
        return;
    }
    isRecording = true;
    Reset();
}

void JzRE::JzRHICommandBuffer::End()
{
    std::lock_guard<std::mutex> lock(commandMutex);
    if (!isRecording) {
        std::cerr << "命令缓冲没有在记录中" << std::endl;
        return;
    }
    isRecording = false;
}

void JzRE::JzRHICommandBuffer::Reset()
{
    std::lock_guard<std::mutex> lock(commandMutex);
    commands.clear();
}

void JzRE::JzRHICommandBuffer::Execute()
{
    std::lock_guard<std::mutex> lock(commandMutex);
    if (isRecording) {
        std::cerr << "不能执行正在记录的命令缓冲" << std::endl;
        return;
    }

    for (auto &command : commands) {
        command->Execute();
    }
}

template <typename T, typename... Args>
void JzRE::JzRHICommandBuffer::AddCommand(Args &&...args)
{
    if (!isRecording) {
        std::cerr << "命令缓冲没有在记录状态" << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(commandMutex);
    commands.push_back(std::make_unique<T>(std::forward<Args>(args)...));
}
