/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/OpenGL/JzOpenGLFramebuffer.h"
#include "JzRE/Runtime/Platform/OpenGL/JzOpenGLPipeline.h"
#include "JzRE/Runtime/Platform/OpenGL/JzOpenGLVertexArray.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Platform/RHI/JzRHICapabilities.h"
#include "JzRE/Runtime/Platform/RHI/JzRHIStats.h"

namespace JzRE {

/**
 * @brief OpenGL implementation of command-list based RHI device.
 */
class JzOpenGLDevice : public JzDevice {
public:
    JzOpenGLDevice();
    ~JzOpenGLDevice() override;

    String GetDeviceName() const override;
    String GetVendorName() const override;
    String GetDriverVersion() const override;

    std::shared_ptr<JzGPUBufferObject>        CreateBuffer(const JzGPUBufferObjectDesc &desc) override;
    std::shared_ptr<JzGPUTextureObject>       CreateTexture(const JzGPUTextureObjectDesc &desc) override;
    std::shared_ptr<JzGPUShaderProgramObject> CreateShader(const JzShaderProgramDesc &desc) override;
    std::shared_ptr<JzRHIPipeline>            CreatePipeline(const JzPipelineDesc &desc) override;
    std::shared_ptr<JzGPUFramebufferObject>   CreateFramebuffer(const String &debugName = "") override;
    std::shared_ptr<JzGPUVertexArrayObject>   CreateVertexArray(const String &debugName = "") override;
    std::shared_ptr<JzRHICommandList>         CreateCommandList(const String &debugName = "") override;

    void ExecuteCommandList(std::shared_ptr<JzRHICommandList> commandList) override;
    void ExecuteCommandLists(const std::vector<std::shared_ptr<JzRHICommandList>> &commandLists) override;

    void BeginFrame() override;
    void EndFrame() override;

    void Flush() override;
    void Finish() override;
    Bool SupportsMultithreading() const override;

    /**
     * @brief Get device capabilities.
     */
    const JzRHICapabilities &GetCapabilities() const;

    /**
     * @brief Get frame statistics.
     */
    JzRHIStats &GetStats();

private:
    void InitializeCapabilities();
    void CheckOpenGLError(const String &operation) const;

    void DispatchCommand(const JzRHIRecordedCommand &command);

    void SetRenderState(const JzRenderState &state);
    void SetViewport(const JzViewport &viewport);
    void SetScissor(const JzScissorRect &scissor);
    void Clear(const JzClearParams &params);
    void Draw(const JzDrawParams &params);
    void DrawIndexed(const JzDrawIndexedParams &params);
    void BindPipeline(std::shared_ptr<JzRHIPipeline> pipeline);
    void BindVertexArray(std::shared_ptr<JzGPUVertexArrayObject> vertexArray);
    void BindTexture(std::shared_ptr<JzGPUTextureObject> texture, U32 slot);
    void BindFramebuffer(std::shared_ptr<JzGPUFramebufferObject> framebuffer);
    void BlitFramebufferToScreen(std::shared_ptr<JzGPUFramebufferObject> framebuffer,
                                 U32 srcWidth, U32 srcHeight,
                                 U32 dstWidth, U32 dstHeight);
    void ResourceBarrier(const std::vector<JzRHIResourceBarrier> &barriers);

    void BeginRenderPass(const JzRHIBeginRenderPassPayload &payload);
    void EndRenderPass(const JzRHIEndRenderPassPayload &payload);

    void ApplyRenderState(const JzRenderState &state);

    static GLenum ConvertPrimitiveType(JzEPrimitiveType type);
    static GLenum ConvertBlendMode(JzEBlendMode mode);
    static GLenum ConvertDepthFunc(JzEDepthFunc func);
    static GLenum ConvertCullMode(JzECullMode mode);

private:
    JzRHICapabilities                    m_capabilities;
    JzRHIStats                           m_stats;
    JzRenderState                        m_currentRenderState;
    std::shared_ptr<JzOpenGLPipeline>    m_currentPipeline;
    std::shared_ptr<JzOpenGLVertexArray> m_currentVertexArray;
    std::shared_ptr<JzOpenGLFramebuffer> m_currentFramebuffer;
};

} // namespace JzRE
