#include "RHICommand.h"
#include "RHI.h"
#include <algorithm>

namespace JzRE {

// RHICommandBuffer实现
RHICommandBuffer::RHICommandBuffer(const String& debugName) 
    : debugName(debugName) {
}

RHICommandBuffer::~RHICommandBuffer() {
    Reset();
}

void RHICommandBuffer::Clear(const ClearParams& params) {
    AddCommand<RHIClearCommand>(params);
}

void RHICommandBuffer::Draw(const DrawParams& params) {
    AddCommand<RHIDrawCommand>(params);
}

void RHICommandBuffer::DrawIndexed(const DrawIndexedParams& params) {
    AddCommand<RHIDrawIndexedCommand>(params);
}

void RHICommandBuffer::BindPipeline(std::shared_ptr<RHIPipeline> pipeline) {
    AddCommand<RHIBindPipelineCommand>(pipeline);
}

void RHICommandBuffer::BindVertexArray(std::shared_ptr<RHIVertexArray> vertexArray) {
    AddCommand<RHIBindVertexArrayCommand>(vertexArray);
}

void RHICommandBuffer::BindTexture(std::shared_ptr<RHITexture> texture, U32 slot) {
    AddCommand<RHIBindTextureCommand>(texture, slot);
}

void RHICommandBuffer::SetViewport(const Viewport& viewport) {
    AddCommand<RHISetViewportCommand>(viewport);
}

void RHICommandBuffer::SetScissor(const ScissorRect& scissor) {
    // TODO: 实现剪裁矩形命令
}

void RHICommandBuffer::BeginRenderPass(std::shared_ptr<RHIFramebuffer> framebuffer) {
    // TODO: 实现渲染通道开始命令
}

void RHICommandBuffer::EndRenderPass() {
    // TODO: 实现渲染通道结束命令
}

void RHICommandBuffer::Begin() {
    std::lock_guard<std::mutex> lock(commandMutex);
    if (isRecording) {
        std::cerr << "命令缓冲已经在记录中" << std::endl;
        return;
    }
    isRecording = true;
    Reset();
}

void RHICommandBuffer::End() {
    std::lock_guard<std::mutex> lock(commandMutex);
    if (!isRecording) {
        std::cerr << "命令缓冲没有在记录中" << std::endl;
        return;
    }
    isRecording = false;
}

void RHICommandBuffer::Reset() {
    std::lock_guard<std::mutex> lock(commandMutex);
    commands.clear();
}

void RHICommandBuffer::Execute() {
    std::lock_guard<std::mutex> lock(commandMutex);
    if (isRecording) {
        std::cerr << "不能执行正在记录的命令缓冲" << std::endl;
        return;
    }
    
    for (auto& command : commands) {
        command->Execute();
    }
}

template<typename T, typename... Args>
void RHICommandBuffer::AddCommand(Args&&... args) {
    if (!isRecording) {
        std::cerr << "命令缓冲没有在记录状态" << std::endl;
        return;
    }
    
    std::lock_guard<std::mutex> lock(commandMutex);
    commands.push_back(std::make_unique<T>(std::forward<Args>(args)...));
}

// 具体命令实现
RHIClearCommand::RHIClearCommand(const ClearParams& params) 
    : RHICommand(ERHICommandType::Clear), params(params) {
}

void RHIClearCommand::Execute() {
    auto device = RHI_DEVICE();
    if (device) {
        device->Clear(params);
    }
}

RHIDrawCommand::RHIDrawCommand(const DrawParams& params)
    : RHICommand(ERHICommandType::Draw), params(params) {
}

void RHIDrawCommand::Execute() {
    auto device = RHI_DEVICE();
    if (device) {
        device->Draw(params);
    }
}

RHIDrawIndexedCommand::RHIDrawIndexedCommand(const DrawIndexedParams& params)
    : RHICommand(ERHICommandType::DrawIndexed), params(params) {
}

void RHIDrawIndexedCommand::Execute() {
    auto device = RHI_DEVICE();
    if (device) {
        device->DrawIndexed(params);
    }
}

RHIBindPipelineCommand::RHIBindPipelineCommand(std::shared_ptr<RHIPipeline> pipeline)
    : RHICommand(ERHICommandType::BindPipeline), pipeline(pipeline) {
}

void RHIBindPipelineCommand::Execute() {
    auto device = RHI_DEVICE();
    if (device) {
        device->BindPipeline(pipeline);
    }
}

RHIBindVertexArrayCommand::RHIBindVertexArrayCommand(std::shared_ptr<RHIVertexArray> vertexArray)
    : RHICommand(ERHICommandType::BindVertexArray), vertexArray(vertexArray) {
}

void RHIBindVertexArrayCommand::Execute() {
    auto device = RHI_DEVICE();
    if (device) {
        device->BindVertexArray(vertexArray);
    }
}

RHIBindTextureCommand::RHIBindTextureCommand(std::shared_ptr<RHITexture> texture, U32 slot)
    : RHICommand(ERHICommandType::BindTexture), texture(texture), slot(slot) {
}

void RHIBindTextureCommand::Execute() {
    auto device = RHI_DEVICE();
    if (device) {
        device->BindTexture(texture, slot);
    }
}

RHISetViewportCommand::RHISetViewportCommand(const Viewport& viewport)
    : RHICommand(ERHICommandType::SetViewport), viewport(viewport) {
}

void RHISetViewportCommand::Execute() {
    auto device = RHI_DEVICE();
    if (device) {
        device->SetViewport(viewport);
    }
}

// RHICommandQueue实现
RHICommandQueue::RHICommandQueue() {
}

RHICommandQueue::~RHICommandQueue() {
    Wait();
}

std::shared_ptr<RHICommandBuffer> RHICommandQueue::CreateCommandBuffer(const String& debugName) {
    return std::make_shared<RHICommandBuffer>(debugName);
}

void RHICommandQueue::SubmitCommandBuffer(std::shared_ptr<RHICommandBuffer> commandBuffer) {
    if (!commandBuffer || commandBuffer->IsEmpty()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(queueMutex);
    pendingCommandBuffers.push_back(commandBuffer);
}

void RHICommandQueue::ExecuteAll() {
    if (isExecuting) {
        return;
    }
    
    isExecuting = true;
    
    std::vector<std::shared_ptr<RHICommandBuffer>> commandBuffersToExecute;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        commandBuffersToExecute = pendingCommandBuffers;
        pendingCommandBuffers.clear();
    }
    
    // 单线程执行所有命令缓冲
    // TODO: 实现多线程并行执行
    for (auto& commandBuffer : commandBuffersToExecute) {
        commandBuffer->Execute();
    }
    
    isExecuting = false;
}

void RHICommandQueue::Wait() {
    while (isExecuting) {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

void RHICommandQueue::SetThreadCount(U32 threadCount) {
    this->threadCount = std::max(1u, threadCount);
}

} // namespace JzRE 