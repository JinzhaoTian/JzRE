#include "JzRHICommand.h"
#include <algorithm>
#include "JzRHI.h"

namespace JzRE {

// RHICommandBuffer实现
JzRHICommandBuffer::RHICommandBuffer(const String &debugName) :
    debugName(debugName)
{
}

JzRHICommandBuffer::~RHICommandBuffer()
{
    Reset();
}

void JzRHICommandBuffer::Clear(const JzClearParams &params)
{
    AddCommand<JzRHIClearCommand>(params);
}

void JzRHICommandBuffer::Draw(const JzDrawParams &params)
{
    AddCommand<JzRHIDrawCommand>(params);
}

void JzRHICommandBuffer::DrawIndexed(const JzDrawIndexedParams &params)
{
    AddCommand<JzRHIDrawIndexedCommand>(params);
}

void JzRHICommandBuffer::BindPipeline(std::shared_ptr<JzRHIPipeline> pipeline)
{
    AddCommand<JzRHIBindPipelineCommand>(pipeline);
}

void JzRHICommandBuffer::BindVertexArray(std::shared_ptr<JzRHIVertexArray> vertexArray)
{
    AddCommand<JzRHIBindVertexArrayCommand>(vertexArray);
}

void JzRHICommandBuffer::BindTexture(std::shared_ptr<JzRHITexture> texture, U32 slot)
{
    AddCommand<JzRHIBindTextureCommand>(texture, slot);
}

void JzRHICommandBuffer::SetViewport(const JzViewport &viewport)
{
    AddCommand<JzRHISetViewportCommand>(viewport);
}

void JzRHICommandBuffer::SetScissor(const JzScissorRect &scissor)
{
    // TODO: 实现剪裁矩形命令
}

void JzRHICommandBuffer::BeginRenderPass(std::shared_ptr<JzRHIFramebuffer> framebuffer)
{
    // TODO: 实现渲染通道开始命令
}

void JzRHICommandBuffer::EndRenderPass()
{
    // TODO: 实现渲染通道结束命令
}

void JzRHICommandBuffer::Begin()
{
    std::lock_guard<std::mutex> lock(commandMutex);
    if (isRecording) {
        std::cerr << "命令缓冲已经在记录中" << std::endl;
        return;
    }
    isRecording = true;
    Reset();
}

void JzRHICommandBuffer::End()
{
    std::lock_guard<std::mutex> lock(commandMutex);
    if (!isRecording) {
        std::cerr << "命令缓冲没有在记录中" << std::endl;
        return;
    }
    isRecording = false;
}

void JzRHICommandBuffer::Reset()
{
    std::lock_guard<std::mutex> lock(commandMutex);
    commands.clear();
}

void JzRHICommandBuffer::Execute()
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
void JzRHICommandBuffer::AddCommand(Args &&...args)
{
    if (!isRecording) {
        std::cerr << "命令缓冲没有在记录状态" << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(commandMutex);
    commands.push_back(std::make_unique<T>(std::forward<Args>(args)...));
}

// 具体命令实现
JzRHIClearCommand::RHIClearCommand(const ClearParams &params) :
    RHICommand(ERHICommandType::Clear), params(params)
{
}

void JzRHIClearCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->Clear(params);
    }
}

JzRHIDrawCommand::RHIDrawCommand(const JzDrawParams &params) :
    RHICommand(JzERHICommandType::Draw), params(params)
{
}

void JzRHIDrawCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->Draw(params);
    }
}

JzRHIDrawIndexedCommand::RHIDrawIndexedCommand(const JzDrawIndexedParams &params) :
    RHICommand(JzERHICommandType::DrawIndexed), params(params)
{
}

void JzRHIDrawIndexedCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->DrawIndexed(params);
    }
}

JzRHIBindPipelineCommand::RHIBindPipelineCommand(std::shared_ptr<JzRHIPipeline> pipeline) :
    RHICommand(JzERHICommandType::BindPipeline), pipeline(pipeline)
{
}

void JzRHIBindPipelineCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->BindPipeline(pipeline);
    }
}

JzRHIBindVertexArrayCommand::RHIBindVertexArrayCommand(std::shared_ptr<JzRHIVertexArray> vertexArray) :
    RHICommand(JzERHICommandType::BindVertexArray), vertexArray(vertexArray)
{
}

void JzRHIBindVertexArrayCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->BindVertexArray(vertexArray);
    }
}

JzRHIBindTextureCommand::RHIBindTextureCommand(std::shared_ptr<JzRHITexture> texture, U32 slot) :
    RHICommand(JzERHICommandType::BindTexture), texture(texture), slot(slot)
{
}

void JzRHIBindTextureCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->BindTexture(texture, slot);
    }
}

JzRHISetViewportCommand::RHISetViewportCommand(const JzViewport &viewport) :
    RHICommand(JzERHICommandType::SetViewport), viewport(viewport)
{
}

void JzRHISetViewportCommand::Execute()
{
    auto device = RHI_DEVICE();
    if (device) {
        device->SetViewport(viewport);
    }
}

// RHICommandQueue实现
JzRHICommandQueue::RHICommandQueue()
{
}

JzRHICommandQueue::~RHICommandQueue()
{
    Wait();
}

std::shared_ptr<JzRHICommandBuffer> JzRHICommandQueue::CreateCommandBuffer(const String &debugName)
{
    return std::make_shared<JzRHICommandBuffer>(debugName);
}

void JzRHICommandQueue::SubmitCommandBuffer(std::shared_ptr<JzRHICommandBuffer> commandBuffer)
{
    if (!commandBuffer || commandBuffer->IsEmpty()) {
        return;
    }

    std::lock_guard<std::mutex> lock(queueMutex);
    pendingCommandBuffers.push_back(commandBuffer);
}

void JzRHICommandQueue::ExecuteAll()
{
    if (isExecuting) {
        return;
    }

    isExecuting = true;

    std::vector<std::shared_ptr<JzRHICommandBuffer>> commandBuffersToExecute;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        commandBuffersToExecute = pendingCommandBuffers;
        pendingCommandBuffers.clear();
    }

    // 单线程执行所有命令缓冲
    // TODO: 实现多线程并行执行
    for (auto &commandBuffer : commandBuffersToExecute) {
        commandBuffer->Execute();
    }

    isExecuting = false;
}

void JzRHICommandQueue::Wait()
{
    while (isExecuting) {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

void JzRHICommandQueue::SetThreadCount(U32 threadCount)
{
    this->threadCount = std::max(1u, threadCount);
}

} // namespace JzRE