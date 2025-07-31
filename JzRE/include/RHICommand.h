#pragma once

#include "RHITypes.h"
#include "RHIResource.h"
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>

namespace JzRE {

/**
 * RHI命令类型
 */
enum class ERHICommandType : U8 {
    Clear,
    Draw,
    DrawIndexed,
    BindPipeline,
    BindVertexArray,
    BindTexture,
    SetUniform,
    SetViewport,
    SetScissor,
    BeginRenderPass,
    EndRenderPass,
    CopyTexture,
    UpdateBuffer
};

/**
 * 视口信息
 */
struct Viewport {
    F32 x = 0.0f;
    F32 y = 0.0f;
    F32 width = 0.0f;
    F32 height = 0.0f;
    F32 minDepth = 0.0f;
    F32 maxDepth = 1.0f;
};

/**
 * 剪裁矩形
 */
struct ScissorRect {
    I32 x = 0;
    I32 y = 0;
    I32 width = 0;
    I32 height = 0;
};

/**
 * 清除参数
 */
struct ClearParams {
    Bool clearColor = false;
    Bool clearDepth = false;
    Bool clearStencil = false;
    F32 colorR = 0.0f;
    F32 colorG = 0.0f;
    F32 colorB = 0.0f;
    F32 colorA = 1.0f;
    F32 depth = 1.0f;
    I32 stencil = 0;
};

/**
 * 绘制参数
 */
struct DrawParams {
    EPrimitiveType primitiveType = EPrimitiveType::Triangles;
    U32 vertexCount = 0;
    U32 instanceCount = 1;
    U32 firstVertex = 0;
    U32 firstInstance = 0;
};

/**
 * 索引绘制参数
 */
struct DrawIndexedParams {
    EPrimitiveType primitiveType = EPrimitiveType::Triangles;
    U32 indexCount = 0;
    U32 instanceCount = 1;
    U32 firstIndex = 0;
    I32 vertexOffset = 0;
    U32 firstInstance = 0;
};

/**
 * RHI命令基类
 */
class RHICommand {
public:
    RHICommand(ERHICommandType type) : type(type) {}
    virtual ~RHICommand() = default;

    ERHICommandType GetType() const { return type; }
    virtual void Execute() = 0;

protected:
    ERHICommandType type;
};

/**
 * RHI命令缓冲
 * 支持命令的记录和回放，为多线程渲染提供基础
 */
class RHICommandBuffer {
public:
    RHICommandBuffer(const String& debugName = "");
    ~RHICommandBuffer();

    // 命令记录接口
    void Clear(const ClearParams& params);
    void Draw(const DrawParams& params);
    void DrawIndexed(const DrawIndexedParams& params);
    void BindPipeline(std::shared_ptr<RHIPipeline> pipeline);
    void BindVertexArray(std::shared_ptr<RHIVertexArray> vertexArray);
    void BindTexture(std::shared_ptr<RHITexture> texture, U32 slot);
    void SetViewport(const Viewport& viewport);
    void SetScissor(const ScissorRect& scissor);
    void BeginRenderPass(std::shared_ptr<RHIFramebuffer> framebuffer);
    void EndRenderPass();

    // 命令缓冲管理
    void Begin();
    void End();
    void Reset();
    void Execute();

    // 多线程支持
    Bool IsRecording() const { return isRecording; }
    Bool IsEmpty() const { return commands.empty(); }
    Size GetCommandCount() const { return commands.size(); }

    const String& GetDebugName() const { return debugName; }

private:
    String debugName;
    std::vector<std::unique_ptr<RHICommand>> commands;
    std::atomic<Bool> isRecording{false};
    mutable std::mutex commandMutex;

    template<typename T, typename... Args>
    void AddCommand(Args&&... args);
};

/**
 * 具体命令实现
 */
class RHIClearCommand : public RHICommand {
public:
    RHIClearCommand(const ClearParams& params);
    void Execute() override;

private:
    ClearParams params;
};

class RHIDrawCommand : public RHICommand {
public:
    RHIDrawCommand(const DrawParams& params);
    void Execute() override;

private:
    DrawParams params;
};

class RHIDrawIndexedCommand : public RHICommand {
public:
    RHIDrawIndexedCommand(const DrawIndexedParams& params);
    void Execute() override;

private:
    DrawIndexedParams params;
};

class RHIBindPipelineCommand : public RHICommand {
public:
    RHIBindPipelineCommand(std::shared_ptr<RHIPipeline> pipeline);
    void Execute() override;

private:
    std::shared_ptr<RHIPipeline> pipeline;
};

class RHIBindVertexArrayCommand : public RHICommand {
public:
    RHIBindVertexArrayCommand(std::shared_ptr<RHIVertexArray> vertexArray);
    void Execute() override;

private:
    std::shared_ptr<RHIVertexArray> vertexArray;
};

class RHIBindTextureCommand : public RHICommand {
public:
    RHIBindTextureCommand(std::shared_ptr<RHITexture> texture, U32 slot);
    void Execute() override;

private:
    std::shared_ptr<RHITexture> texture;
    U32 slot;
};

class RHISetViewportCommand : public RHICommand {
public:
    RHISetViewportCommand(const Viewport& viewport);
    void Execute() override;

private:
    Viewport viewport;
};

/**
 * 命令队列管理器
 * 支持多线程命令提交和执行
 */
class RHICommandQueue {
public:
    RHICommandQueue();
    ~RHICommandQueue();

    // 命令缓冲管理
    std::shared_ptr<RHICommandBuffer> CreateCommandBuffer(const String& debugName = "");
    void SubmitCommandBuffer(std::shared_ptr<RHICommandBuffer> commandBuffer);
    void ExecuteAll();
    void Wait();

    // 多线程支持
    void SetThreadCount(U32 threadCount);
    U32 GetThreadCount() const { return threadCount; }

private:
    std::vector<std::shared_ptr<RHICommandBuffer>> pendingCommandBuffers;
    std::mutex queueMutex;
    std::atomic<U32> threadCount{1};
    std::atomic<Bool> isExecuting{false};
};

} // namespace JzRE 