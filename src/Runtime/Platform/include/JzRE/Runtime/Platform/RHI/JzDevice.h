/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/Command/JzRHICommandList.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUBufferObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUFramebufferObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUVertexArrayObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUShaderProgramObject.h"
#include "JzRE/Runtime/Platform/RHI/JzRHIPipeline.h"

namespace JzRE {

/**
 * @brief Supported graphics API types
 */
enum class JzERHIType : U8 {
    Unknown = 0,
    OpenGL,
    Vulkan,
    D3D11,
    D3D12,
    Metal
};

/**
 * @brief Render mode
 */
enum class JzERenderMode : U8 {
    Immediate,
    CommandBuffer
};

/**
 * @brief Interface of RHI Device
 */
class JzDevice {
public:
    /**
     * @brief Constructor
     *
     * @param rhiType
     */
    JzDevice(JzERHIType rhiType) :
        rhiType(rhiType) { }

    /**
     * @brief Destructor
     */
    virtual ~JzDevice() = default;

    /**
     * @brief Get the RHI Type
     *
     * @return JzERHIType
     */
    JzERHIType GetRHIType() const
    {
        return rhiType;
    }

    /**
     * @brief Get the Device Name
     *
     * @return String
     */
    virtual String GetDeviceName() const = 0;

    /**
     * @brief Get the Vendor Name
     *
     * @return String
     */
    virtual String GetVendorName() const = 0;

    /**
     * @brief Get the Driver Version
     *
     * @return String
     */
    virtual String GetDriverVersion() const = 0;

    /**
     * @brief Create a Buffer
     *
     * @param desc Buffer Description
     * @return std::shared_ptr<JzGPUBufferObject>
     */
    virtual std::shared_ptr<JzGPUBufferObject> CreateBuffer(const JzGPUBufferObjectDesc &desc) = 0;

    /**
     * @brief Create a Texture
     *
     * @param desc Texture Description
     * @return std::shared_ptr<JzGPUTextureObject>
     */
    virtual std::shared_ptr<JzGPUTextureObject> CreateTexture(const JzGPUTextureObjectDesc &desc) = 0;

    /**
     * @brief Create a Shader
     *
     * @param desc Shader Description
     * @return std::shared_ptr<JzGPUShaderProgramObject>
     */
    virtual std::shared_ptr<JzGPUShaderProgramObject> CreateShader(const JzShaderProgramDesc &desc) = 0;

    /**
     * @brief Create a Pipeline
     *
     * @param desc Pipeline Description
     * @return std::shared_ptr<JzRHIPipeline>
     */
    virtual std::shared_ptr<JzRHIPipeline> CreatePipeline(const JzPipelineDesc &desc) = 0;

    /**
     * @brief Create a Framebuffer
     *
     * @param debugName Debug Name
     * @return std::shared_ptr<JzGPUFramebufferObject>
     */
    virtual std::shared_ptr<JzGPUFramebufferObject> CreateFramebuffer(const String &debugName = "") = 0;

    /**
     * @brief Create a Vertex Array
     *
     * @param debugName Debug Name
     * @return std::shared_ptr<JzGPUVertexArrayObject>
     */
    virtual std::shared_ptr<JzGPUVertexArrayObject> CreateVertexArray(const String &debugName = "") = 0;

    /**
     * @brief Create a Command List
     *
     * @param debugName Debug Name
     * @return std::shared_ptr<JzRHICommandList>
     */
    virtual std::shared_ptr<JzRHICommandList> CreateCommandList(const String &debugName = "") = 0;

    /**
     * @brief Execute a Command List
     *
     * @param commandBuffer Command List
     */
    virtual void ExecuteCommandList(std::shared_ptr<JzRHICommandList> commandList) = 0;

    /**
     * @brief Begin a Frame
     */
    virtual void BeginFrame() = 0;

    /**
     * @brief End a Frame
     */
    virtual void EndFrame() = 0;

    /**
     * @brief Set Render State
     *
     * @param state Render State
     */
    virtual void SetRenderState(const JzRenderState &state) = 0;

    /**
     * @brief Set Viewport
     *
     * @param viewport Viewport
     */
    virtual void SetViewport(const JzViewport &viewport) = 0;

    /**
     * @brief Set Scissor
     *
     * @param scissor Scissor
     */
    virtual void SetScissor(const JzScissorRect &scissor) = 0;

    /**
     * @brief Clear
     *
     * @param params Clear Parameters
     */
    virtual void Clear(const JzClearParams &params) = 0;

    /**
     * @brief Draw
     *
     * @param params Draw Parameters
     */
    virtual void Draw(const JzDrawParams &params) = 0;

    /**
     * @brief Draw Indexed
     *
     * @param params Draw Indexed Parameters
     */
    virtual void DrawIndexed(const JzDrawIndexedParams &params) = 0;

    /**
     * @brief Bind Pipeline
     *
     * @param pipeline Pipeline
     */
    virtual void BindPipeline(std::shared_ptr<JzRHIPipeline> pipeline) = 0;

    /**
     * @brief Bind Vertex Array
     *
     * @param vertexArray Vertex Array
     */
    virtual void BindVertexArray(std::shared_ptr<JzGPUVertexArrayObject> vertexArray) = 0;

    /**
     * @brief Bind Texture
     *
     * @param texture Texture
     * @param slot Slot
     */
    virtual void BindTexture(std::shared_ptr<JzGPUTextureObject> texture, U32 slot) = 0;

    /**
     * @brief Bind Framebuffer
     *
     * @param framebuffer Framebuffer
     */
    virtual void BindFramebuffer(std::shared_ptr<JzGPUFramebufferObject> framebuffer) = 0;

    /**
     * @brief Blit framebuffer content to the default framebuffer (screen)
     *
     * @param framebuffer Source framebuffer to blit from
     * @param srcWidth Source width
     * @param srcHeight Source height
     * @param dstWidth Destination width
     * @param dstHeight Destination height
     */
    virtual void BlitFramebufferToScreen(std::shared_ptr<JzGPUFramebufferObject> framebuffer,
                                         U32 srcWidth, U32 srcHeight,
                                         U32 dstWidth, U32 dstHeight) = 0;

    /**
     * @brief Flush
     */
    virtual void Flush() = 0;

    /**
     * @brief Finish
     */
    virtual void Finish() = 0;

    /**
     * @brief Supports Multithreading
     *
     * @return Bool
     */
    virtual Bool SupportsMultithreading() const = 0;

protected:
    JzERHIType rhiType;
};
} // namespace JzRE
