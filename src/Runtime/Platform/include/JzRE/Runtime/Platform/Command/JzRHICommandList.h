/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <mutex>
#include <variant>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/Command/JzRHICommand.h"
#include "JzRE/Runtime/Platform/Command/JzRHIClearCommand.h"
#include "JzRE/Runtime/Platform/Command/JzRHIDrawCommand.h"
#include "JzRE/Runtime/Platform/Command/JzRHIDrawIndexedCommand.h"
#include "JzRE/Runtime/Platform/Command/JzRHISetViewportCommand.h"
#include "JzRE/Runtime/Platform/Command/JzRHISetScissorCommand.h"
#include "JzRE/Runtime/Platform/RHI/JzRHIResourceBarrier.h"
#include "JzRE/Runtime/Platform/RHI/JzRHIRenderPass.h"
#include "JzRE/Runtime/Platform/RHI/JzRHIPipeline.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUFramebufferObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUVertexArrayObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"

namespace JzRE {

/**
 * @brief Payload for pipeline binding command.
 */
struct JzRHIBindPipelinePayload {
    std::shared_ptr<JzRHIPipeline> pipeline;
};

/**
 * @brief Payload for vertex array binding command.
 */
struct JzRHIBindVertexArrayPayload {
    std::shared_ptr<JzGPUVertexArrayObject> vertexArray;
};

/**
 * @brief Payload for texture binding command.
 */
struct JzRHIBindTexturePayload {
    std::shared_ptr<JzGPUTextureObject> texture;
    U32                                 slot = 0;
};

/**
 * @brief Payload for framebuffer binding command.
 */
struct JzRHIBindFramebufferPayload {
    std::shared_ptr<JzGPUFramebufferObject> framebuffer;
};

/**
 * @brief Payload for barrier command.
 */
struct JzRHIResourceBarrierPayload {
    std::vector<JzRHIResourceBarrier> barriers;
};

/**
 * @brief Payload for framebuffer blit-to-screen command.
 */
struct JzRHIBlitFramebufferToScreenPayload {
    std::shared_ptr<JzGPUFramebufferObject> framebuffer;
    U32                                     srcWidth  = 0;
    U32                                     srcHeight = 0;
    U32                                     dstWidth  = 0;
    U32                                     dstHeight = 0;
};

/**
 * @brief Payload for begin render pass command.
 */
struct JzRHIBeginRenderPassPayload {
    std::shared_ptr<JzGPUFramebufferObject> framebuffer;
    std::shared_ptr<JzRHIRenderPass>        renderPass;
};

/**
 * @brief Payload for end render pass command.
 */
struct JzRHIEndRenderPassPayload {
    std::shared_ptr<JzRHIRenderPass> renderPass;
};

/**
 * @brief Variant payload used by recorded commands.
 */
using JzRHICommandPayload = std::variant<
    std::monostate,
    JzClearParams,
    JzDrawParams,
    JzDrawIndexedParams,
    JzViewport,
    JzScissorRect,
    JzRHIBindPipelinePayload,
    JzRHIBindVertexArrayPayload,
    JzRHIBindTexturePayload,
    JzRHIBindFramebufferPayload,
    JzRHIResourceBarrierPayload,
    JzRHIBlitFramebufferToScreenPayload,
    JzRHIBeginRenderPassPayload,
    JzRHIEndRenderPassPayload>;

/**
 * @brief Recorded RHI command with type + payload.
 */
struct JzRHIRecordedCommand {
    JzRHIECommandType type    = JzRHIECommandType::Clear;
    JzRHICommandPayload payload = std::monostate{};
};

/**
 * @brief RHI Command List, Supports command recording and playback
 */
class JzRHICommandList {
public:
    /**
     * @brief Constructor
     *
     * @param debugName The debug name of the command buffer
     */
    JzRHICommandList(const String &debugName = "");

    /**
     * @brief Destructor
     */
    ~JzRHICommandList();

    /**
     * @brief Begin Recording Commands
     */
    void Begin();

    /**
     * @brief End Recording Commands
     */
    void End();

    /**
     * @brief Reset the command buffer
     */
    void Reset();

    /**
     * @brief Execute the command buffer
     */
    std::vector<JzRHIRecordedCommand> GetCommands() const;

    /**
     * @brief Check if the command buffer is empty
     */
    Bool IsEmpty() const;

    /**
     * @brief Get the command count
     */
    Size GetCommandCount() const;

    /**
     * @brief Get the debug name of the command buffer
     */
    const String &GetDebugName() const;

    /**
     * @brief Check if the command buffer is recording.
     */
    Bool IsRecording() const;

    /**
     * @brief Buffer Clear Command
     *
     * @param params The parameters of the clear command
     */
    void Clear(const JzClearParams &params);

    /**
     * @brief Buffer Draw Command
     *
     * @param params The parameters of the draw command
     */
    void Draw(const JzDrawParams &params);

    /**
     * @brief Buffer Draw Indexed Command
     *
     * @param params The parameters of the draw indexed command
     */
    void DrawIndexed(const JzDrawIndexedParams &params);

    /**
     * @brief Buffer Bind Pipeline Command
     *
     * @param pipeline The pipeline to bind
     */
    void BindPipeline(std::shared_ptr<JzRHIPipeline> pipeline);

    /**
     * @brief Buffer Bind Vertex Array Command
     *
     * @param vertexArray The vertex array to bind
     */
    void BindVertexArray(std::shared_ptr<JzGPUVertexArrayObject> vertexArray);

    /**
     * @brief Buffer Bind Texture Command
     *
     * @param texture The texture to bind
     * @param slot The slot to bind the texture to
     */
    void BindTexture(std::shared_ptr<JzGPUTextureObject> texture, U32 slot);

    /**
     * @brief Buffer Bind Framebuffer Command
     *
     * @param framebuffer The framebuffer to bind
     */
    void BindFramebuffer(std::shared_ptr<JzGPUFramebufferObject> framebuffer);

    /**
     * @brief Buffer Set Viewport Command
     *
     * @param viewport The viewport to set
     */
    void SetViewport(const JzViewport &viewport);

    /**
     * @brief Buffer Set Scissor Command
     *
     * @param scissor The scissor to set
     */
    void SetScissor(const JzScissorRect &scissor);

    /**
     * @brief Buffer Resource Barrier Command
     *
     * @param barriers Barrier list
     */
    void ResourceBarrier(const std::vector<JzRHIResourceBarrier> &barriers);

    /**
     * @brief Buffer Blit Framebuffer To Screen Command
     *
     * @param framebuffer Source framebuffer
     * @param srcWidth Source width
     * @param srcHeight Source height
     * @param dstWidth Destination width
     * @param dstHeight Destination height
     */
    void BlitFramebufferToScreen(std::shared_ptr<JzGPUFramebufferObject> framebuffer,
                                 U32 srcWidth, U32 srcHeight,
                                 U32 dstWidth, U32 dstHeight);

    /**
     * @brief Buffer Begin Render Pass Command
     * @param framebuffer The framebuffer to begin the render pass
     */
    void BeginRenderPass(std::shared_ptr<JzGPUFramebufferObject> framebuffer);

    /**
     * @brief Buffer Begin Render Pass Command
     *
     * @param renderPass Render pass metadata
     * @param framebuffer The framebuffer to begin the render pass
     */
    void BeginRenderPass(std::shared_ptr<JzRHIRenderPass>        renderPass,
                         std::shared_ptr<JzGPUFramebufferObject> framebuffer);

    /**
     * @brief Buffer End Render Pass Command
     */
    void EndRenderPass();

    /**
     * @brief Buffer End Render Pass Command
     *
     * @param renderPass Render pass metadata
     */
    void EndRenderPass(std::shared_ptr<JzRHIRenderPass> renderPass);

private:
    template <typename TPayload>
    void AddCommand(JzRHIECommandType type, TPayload &&payload);

private:
    String                           m_debugName;
    std::vector<JzRHIRecordedCommand> m_commands;
    Bool                              m_isRecording{false};
    mutable std::mutex                m_commandMutex;
};
} // namespace JzRE
