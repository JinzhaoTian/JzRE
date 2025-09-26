/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRETypes.h"
#include "JzRHIETypes.h"
#include "JzRHIFramebuffer.h"
#include "JzRHIPipeline.h"
#include "JzRHIResource.h"
#include "JzRHITexture.h"
#include "JzRHIVertexArray.h"

namespace JzRE {

/**
 * @brief Enums of RHI command type
 */
enum class JzRHIECommandType : U8 {
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
 * @brief RHI Command Interface
 */
class JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param type The type of the command
     */
    JzRHICommand(JzRHIECommandType type) :
        type(type) { }

    /**
     * @brief Destructor
     */
    virtual ~JzRHICommand() = default;

    /**
     * @brief Get the type of the command
     * @return The type of the command
     */
    JzRHIECommandType GetType() const
    {
        return type;
    }

    /**
     * @brief Execute the command
     */
    virtual void Execute() = 0;

protected:
    JzRHIECommandType type;
};

/**
 * @brief Clear Command
 */
class JzRHIClearCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param params The parameters of the command
     */
    JzRHIClearCommand(const JzClearParams &params);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    JzClearParams m_params;
};

/**
 * @brief Draw Command
 */
class JzRHIDrawCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param params The parameters of the command
     */
    JzRHIDrawCommand(const JzDrawParams &params);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    JzDrawParams m_params;
};

/**
 * @brief Draw Indexed Command
 */
class JzRHIDrawIndexedCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param params The parameters of the command
     */
    JzRHIDrawIndexedCommand(const JzDrawIndexedParams &params);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    JzDrawIndexedParams m_params;
};

/**
 * @brief Bind Pipeline Command
 */
class JzRHIBindPipelineCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param pipeline The pipeline to bind
     */
    JzRHIBindPipelineCommand(std::shared_ptr<JzRHIPipeline> pipeline);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    std::shared_ptr<JzRHIPipeline> m_pipeline;
};

/**
 * @brief Bind Vertex Array Command
 */
class JzRHIBindVertexArrayCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param vertexArray The vertex array to bind
     */
    JzRHIBindVertexArrayCommand(std::shared_ptr<JzRHIVertexArray> vertexArray);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    std::shared_ptr<JzRHIVertexArray> m_vertexArray;
};

/**
 * @brief Bind Texture Command
 */
class JzRHIBindTextureCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param texture The texture to bind
     * @param slot The slot to bind the texture to
     */
    JzRHIBindTextureCommand(std::shared_ptr<JzRHITexture> texture, U32 slot);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    std::shared_ptr<JzRHITexture> m_texture;
    U32                           m_slot;
};

/**
 * @brief Set Viewport Command
 */
class JzRHISetViewportCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param viewport The viewport to set
     */
    JzRHISetViewportCommand(const JzViewport &viewport);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    JzViewport m_viewport;
};

/**
 * @brief Set Scissor Command
 */
class JzRHISetScissorCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param scissor The scissor to set
     */
    JzRHISetScissorCommand(const JzScissorRect &scissor);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    JzScissorRect m_scissorRect;
};

/**
 * @brief Begin Render Pass Command
 */
class JzRHIBeginRenderPassCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param renderPass The render pass to begin
     */
    JzRHIBeginRenderPassCommand(std::shared_ptr<JzRHIFramebuffer> framebuffer);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    std::shared_ptr<JzRHIFramebuffer> m_framebuffer;
};

/**
 * @brief End Render Pass Command
 */
class JzRHIEndRenderPassCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     */
    JzRHIEndRenderPassCommand();

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    std::shared_ptr<JzRHIFramebuffer> m_framebuffer;
};

// /**
//  * @brief Copy Texture Command
//  */
// class JzRHICopyTextureCommand : public JzRHICommand {
// public:
//     /**
//      * @brief Constructor
//      * @param sourceTexture The source texture to copy from
//      * @param destinationTexture The destination texture to copy to
//      * @param sourceRegion The region of the source texture to copy from
//      * @param destinationRegion The region of the destination texture to copy to
//      */
//     JzRHICopyTextureCommand(std::shared_ptr<JzRHITexture> sourceTexture, std::shared_ptr<JzRHITexture> destinationTexture, const JzRect &sourceRegion, const JzRect &destinationRegion);

//     /**
//      * @brief Execute the command
//      */
//     void Execute() override;

// private:
//     std::shared_ptr<JzRHITexture> sourceTexture;
//     std::shared_ptr<JzRHITexture> destinationTexture;
//     JzRect                        sourceRegion;
//     JzRect                        destinationRegion;
// };

/**
 * @brief Update Buffer Command
 */
class JzRHIUpdateBufferCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param buffer The buffer to update
     */
    JzRHIUpdateBufferCommand(std::shared_ptr<JzRHIBuffer> buffer, const void *data, U64 size);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    std::shared_ptr<JzRHIBuffer> m_buffer;
    const void                  *m_data;
    U64                          m_size;
};

} // namespace JzRE