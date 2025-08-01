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