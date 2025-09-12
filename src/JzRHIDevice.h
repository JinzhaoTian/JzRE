/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRETypes.h"
#include "JzRHICommandList.h"
#include "JzRHIDescription.h"
#include "JzRHIETypes.h"
#include "JzRHIFramebuffer.h"
#include "JzRHIShader.h"

namespace JzRE {
/**
 * @brief Interface of RHI Device
 */
class JzRHIDevice {
public:
    /**
     * @brief Constructor
     *
     * @param rhiType
     */
    JzRHIDevice(JzERHIType rhiType) :
        rhiType(rhiType) { }

    /**
     * @brief Destructor
     */
    virtual ~JzRHIDevice() = default;

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
     * @return std::shared_ptr<JzRHIBuffer>
     */
    virtual std::shared_ptr<JzRHIBuffer> CreateBuffer(const JzBufferDesc &desc) = 0;

    /**
     * @brief Create a Texture
     *
     * @param desc Texture Description
     * @return std::shared_ptr<JzRHITexture>
     */
    virtual std::shared_ptr<JzRHITexture> CreateTexture(const JzTextureDesc &desc) = 0;

    /**
     * @brief Create a Shader
     *
     * @param desc Shader Description
     * @return std::shared_ptr<JzRHIShader>
     */
    virtual std::shared_ptr<JzRHIShader> CreateShader(const JzShaderDesc &desc) = 0;

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
     * @return std::shared_ptr<JzRHIFramebuffer>
     */
    virtual std::shared_ptr<JzRHIFramebuffer> CreateFramebuffer(const String &debugName = "") = 0;

    /**
     * @brief Create a Vertex Array
     *
     * @param debugName Debug Name
     * @return std::shared_ptr<JzRHIVertexArray>
     */
    virtual std::shared_ptr<JzRHIVertexArray> CreateVertexArray(const String &debugName = "") = 0;

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
     * @brief Present
     */
    virtual void Present() = 0;

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
    virtual void BindVertexArray(std::shared_ptr<JzRHIVertexArray> vertexArray) = 0;

    /**
     * @brief Bind Texture
     *
     * @param texture Texture
     * @param slot Slot
     */
    virtual void BindTexture(std::shared_ptr<JzRHITexture> texture, U32 slot) = 0;

    /**
     * @brief Bind Framebuffer
     *
     * @param framebuffer Framebuffer
     */
    virtual void BindFramebuffer(std::shared_ptr<JzRHIFramebuffer> framebuffer) = 0;

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

    /**
     * @brief Make Context Current
     *
     * @param threadIndex Thread Index
     */
    virtual void MakeContextCurrent(U32 threadIndex = 0) = 0;

protected:
    JzERHIType rhiType;
};
} // namespace JzRE