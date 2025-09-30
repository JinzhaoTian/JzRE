/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <mutex>
#include "JzRE/Core/JzRETypes.h"
#include "JzRE/RHI/JzRHICommand.h"
#include "JzRE/RHI/JzRHIClearCommand.h"
#include "JzRE/RHI/JzRHIDrawCommand.h"
#include "JzRE/RHI/JzRHIDrawIndexedCommand.h"
#include "JzRE/RHI/JzRHISetViewportCommand.h"
#include "JzRE/RHI/JzRHISetScissorCommand.h"
#include "JzRE/RHI/JzRHIPipeline.h"
#include "JzRE/RHI/JzRHIFramebuffer.h"
#include "JzRE/RHI/JzRHIVertexArray.h"

namespace JzRE {
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
    void Execute();

    /**
     * @brief Check if the command buffer is recording
     */
    Bool IsRecording() const;

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
    void BindVertexArray(std::shared_ptr<JzRHIVertexArray> vertexArray);

    /**
     * @brief Buffer Bind Texture Command
     *
     * @param texture The texture to bind
     * @param slot The slot to bind the texture to
     */
    void BindTexture(std::shared_ptr<JzRHITexture> texture, U32 slot);

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
     * @brief Buffer Begin Render Pass Command
     * @param framebuffer The framebuffer to begin the render pass
     */
    void BeginRenderPass(std::shared_ptr<JzRHIFramebuffer> framebuffer);

    /**
     * @brief Buffer End Render Pass Command
     */
    void EndRenderPass();

private:
    template <typename T, typename... Args>
    void AddCommand(Args &&...args);

private:
    String                                     m_debugName;
    std::vector<std::unique_ptr<JzRHICommand>> m_commands;
    std::atomic<Bool>                          m_isRecording{false};
    mutable std::mutex                         m_commandMutex;
};
} // namespace JzRE