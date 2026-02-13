/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"

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
 * @brief Primitive type
 */
enum class JzEPrimitiveType : U8 {
    Points,
    Lines,
    LineStrip,
    Triangles,
    TriangleStrip,
    TriangleFan
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

} // namespace JzRE
