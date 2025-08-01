#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"
#include "JzRHIPipeline.h"
#include "JzRHIResource.h"
#include "JzRHITexture.h"
#include "JzRHIVertexArray.h"

namespace JzRE {

/**
 * @brief RHI command type
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
 * @brief Viewport information
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
 * @brief Scissor rectangle
 */
struct JzScissorRect {
    I32 x      = 0;
    I32 y      = 0;
    I32 width  = 0;
    I32 height = 0;
};

/**
 * @brief Clear parameters
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
 * @brief Draw parameters
 */
struct JzDrawParams {
    JzEPrimitiveType primitiveType = JzEPrimitiveType::Triangles;
    U32              vertexCount   = 0;
    U32              instanceCount = 1;
    U32              firstVertex   = 0;
    U32              firstInstance = 0;
};

/**
 * @brief Indexed draw parameters
 */
struct JzDrawIndexedParams {
    JzEPrimitiveType primitiveType = JzEPrimitiveType::Triangles;
    U32              indexCount    = 0;
    U32              instanceCount = 1;
    U32              firstIndex    = 0;
    I32              vertexOffset  = 0;
    U32              firstInstance = 0;
};

/**
 * @brief RHI command base class
 */
class JzRHICommand {
public:
    JzRHICommand(JzERHICommandType type);
    virtual ~JzRHICommand() = default;

    JzERHICommandType GetType() const;
    virtual void      Execute() = 0;

protected:
    JzERHICommandType type;
};

/**
 * @brief Specific command implementation
 */
class JzRHIClearCommand : public JzRHICommand {
public:
    JzRHIClearCommand(const JzClearParams &params);
    void Execute() override;

private:
    JzClearParams params;
};

/**
 * @brief Draw command implementation
 */
class JzRHIDrawCommand : public JzRHICommand {
public:
    JzRHIDrawCommand(const JzDrawParams &params);
    void Execute() override;

private:
    JzDrawParams params;
};

/**
 * @brief Indexed draw command implementation
 */
class JzRHIDrawIndexedCommand : public JzRHICommand {
public:
    JzRHIDrawIndexedCommand(const JzDrawIndexedParams &params);
    void Execute() override;

private:
    JzDrawIndexedParams params;
};

/**
 * @brief Bind pipeline command implementation
 */
class JzRHIBindPipelineCommand : public JzRHICommand {
public:
    JzRHIBindPipelineCommand(std::shared_ptr<JzRHIPipeline> pipeline);
    void Execute() override;

private:
    std::shared_ptr<JzRHIPipeline> pipeline;
};

/**
 * @brief Bind vertex array command implementation
 */
class JzRHIBindVertexArrayCommand : public JzRHICommand {
public:
    JzRHIBindVertexArrayCommand(std::shared_ptr<JzRHIVertexArray> vertexArray);
    void Execute() override;

private:
    std::shared_ptr<JzRHIVertexArray> vertexArray;
};

/**
 * @brief Bind texture command implementation
 */
class JzRHIBindTextureCommand : public JzRHICommand {
public:
    JzRHIBindTextureCommand(std::shared_ptr<JzRHITexture> texture, U32 slot);
    void Execute() override;

private:
    std::shared_ptr<JzRHITexture> texture;
    U32                           slot;
};

/**
 * @brief Set viewport command implementation
 */
class JzRHISetViewportCommand : public JzRHICommand {
public:
    JzRHISetViewportCommand(const JzViewport &viewport);
    void Execute() override;

private:
    JzViewport viewport;
};

} // namespace JzRE