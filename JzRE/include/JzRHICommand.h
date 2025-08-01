#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"
#include "JzRHIPipeline.h"
#include "JzRHIResource.h"
#include "JzRHITexture.h"
#include "JzRHIVertexArray.h"

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
struct JzDrawIndexedParams {
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
    JzRHICommand(JzERHICommandType type);
    virtual ~JzRHICommand() = default;

    JzERHICommandType GetType() const;
    virtual void Execute() = 0;

protected:
    JzERHICommandType type;
};

/**
 * 具体命令实现
 */
class JzRHIClearCommand : public JzRHICommand {
public:
    JzRHIClearCommand(const JzClearParams &params);
    void Execute() override;

private:
    JzClearParams params;
};

class JzRHIDrawCommand : public JzRHICommand {
public:
    JzRHIDrawCommand(const JzDrawParams &params);
    void Execute() override;

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

} // namespace JzRE