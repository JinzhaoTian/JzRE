#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include "JzRHIResource.h"
#include "JzRHITypes.h"

namespace JzRE {

/**
 * RHI命令类型
 */
enum class JzERHICommandType : U8 {
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
struct JzViewport {
    F32 x        = 0.0f;
    F32 y        = 0.0f;
    F32 width    = 0.0f;
    F32 height   = 0.0f;
    F32 minDepth = 0.0f;
    F32 maxDepth = 1.0f;
};

/**
 * 剪裁矩形
 */
struct JzScissorRect {
    I32 x      = 0;
    I32 y      = 0;
    I32 width  = 0;
    I32 height = 0;
};

/**
 * 清除参数
 */
struct JzClearParams {
    Bool clearColor   = false;
    Bool clearDepth   = false;
    Bool clearStencil = false;
    F32  colorR       = 0.0f;
    F32  colorG       = 0.0f;
    F32  colorB       = 0.0f;
    F32  colorA       = 1.0f;
    F32  depth        = 1.0f;
    I32  stencil      = 0;
};

/**
 * 绘制参数
 */
struct JzDrawParams {
    JzEPrimitiveType primitiveType = JzEPrimitiveType::Triangles;
    U32              vertexCount   = 0;
    U32              instanceCount = 1;
    U32              firstVertex   = 0;
    U32              firstInstance = 0;
};

/**
 * 索引绘制参数
 */
struct DrawIndexedParams {
    JzEPrimitiveType primitiveType = JzEPrimitiveType::Triangles;
    U32              indexCount    = 0;
    U32              instanceCount = 1;
    U32              firstIndex    = 0;
    I32              vertexOffset  = 0;
    U32              firstInstance = 0;
};

/**
 * RHI命令基类
 */
class JzRHICommand {
public:
    JzRHICommand(JzERHICommandType type) :
        type(type) { }
    virtual ~JzRHICommand() = default;

    JzERHICommandType GetType() const
    {
        return type;
    }
    virtual void Execute() = 0;

protected:
    JzERHICommandType type;
};

/**
 * RHI命令缓冲
 * 支持命令的记录和回放，为多线程渲染提供基础
 */
class JzRHICommandBuffer {
public:
    JzRHICommandBuffer(const String &debugName = "");
    ~JzRHICommandBuffer();

    // 命令记录接口
    void Clear(const JzClearParams &params);
    void Draw(const JzDrawParams &params);
    void DrawIndexed(const JzDrawIndexedParams &params);
    void BindPipeline(std::shared_ptr<JzRHIPipeline> pipeline);
    void BindVertexArray(std::shared_ptr<JzRHIVertexArray> vertexArray);
    void BindTexture(std::shared_ptr<JzRHITexture> texture, U32 slot);
    void SetViewport(const JzViewport &viewport);
    void SetScissor(const JzScissorRect &scissor);
    void BeginRenderPass(std::shared_ptr<JzRHIFramebuffer> framebuffer);
    void EndRenderPass();

    // 命令缓冲管理
    void Begin();
    void End();
    void Reset();
    void Execute();

    // 多线程支持
    Bool IsRecording() const
    {
        return isRecording;
    }
    Bool IsEmpty() const
    {
        return commands.empty();
    }
    Size GetCommandCount() const
    {
        return commands.size();
    }

    const String &GetDebugName() const
    {
        return debugName;
    }

private:
    String                                     debugName;
    std::vector<std::unique_ptr<JzRHICommand>> commands;
    std::atomic<Bool>                          isRecording{false};
    mutable std::mutex                         commandMutex;

    template <typename T, typename... Args>
    void AddCommand(Args &&...args);
};

/**
 * 具体命令实现
 */
class JzRHIClearCommand : public JzRHICommand {
public:
    JzRHIClearCommand(const JzClearParams &params);
    void Execute() override;

private:
    ClearParams params;
};

class JzRHIDrawCommand : public JzRHICommand {
public:
    JzRHIDrawCommand(const JzDrawParams &params);
    void JzExecute() override;

private:
    JzDrawParams params;
};

class JzRHIDrawIndexedCommand : public JzRHICommand {
public:
    JzRHIDrawIndexedCommand(const JzDrawIndexedParams &params);
    void Execute() override;

private:
    JzDrawIndexedParams params;
};

class JzRHIBindPipelineCommand : public JzRHICommand {
public:
    JzRHIBindPipelineCommand(std::shared_ptr<JzRHIPipeline> pipeline);
    void Execute() override;

private:
    std::shared_ptr<JzRHIPipeline> pipeline;
};

class JzRHIBindVertexArrayCommand : public JzRHICommand {
public:
    JzRHIBindVertexArrayCommand(std::shared_ptr<JzRHIVertexArray> vertexArray);
    void Execute() override;

private:
    std::shared_ptr<JzRHIVertexArray> vertexArray;
};

class JzRHIBindTextureCommand : public JzRHICommand {
public:
    JzRHIBindTextureCommand(std::shared_ptr<JzRHITexture> texture, U32 slot);
    void Execute() override;

private:
    std::shared_ptr<JzRHITexture> texture;
    U32                           slot;
};

class JzRHISetViewportCommand : public JzRHICommand {
public:
    JzRHISetViewportCommand(const JzViewport &viewport);
    void Execute() override;

private:
    JzViewport viewport;
};

/**
 * 命令队列管理器
 * 支持多线程命令提交和执行
 */
class JzRHICommandQueue {
public:
    JzRHICommandQueue();
    ~JzRHICommandQueue();

    // 命令缓冲管理
    std::shared_ptr<JzRHICommandBuffer> CreateCommandBuffer(const String &debugName = "");
    void                                SubmitCommandBuffer(std::shared_ptr<JzRHICommandBuffer> commandBuffer);
    void                                ExecuteAll();
    void                                Wait();

    // 多线程支持
    void SetThreadCount(U32 threadCount);
    U32  GetThreadCount() const
    {
        return threadCount;
    }

private:
    std::vector<std::shared_ptr<JzRHICommandBuffer>> pendingCommandBuffers;
    std::mutex                                       queueMutex;
    std::atomic<U32>                                 threadCount{1};
    std::atomic<Bool>                                isExecuting{false};
};

} // namespace JzRE