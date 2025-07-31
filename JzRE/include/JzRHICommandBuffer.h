#pragma once

#include "CommonTypes.h"
#include "JzRHICommand.h"
#include "JzRHIETypes.h"
#include "JzRHIFramebuffer.h"

namespace JzRE {
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
} // namespace JzRE