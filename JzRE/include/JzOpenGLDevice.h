#pragma once

#include "CommonTypes.h"
#include "JzOpenGLFramebuffer.h"
#include "JzOpenGLPipeline.h"
#include "JzOpenGLVertexArray.h"
#include "JzRHICapabilities.h"
#include "JzRHIDevice.h"
#include "JzRHIStats.h"

namespace JzRE {
/**
 * @brief OpenGL Implementation of RHI Device
 */
class JzOpenGLDevice : public JzRHIDevice {
public:
    /**
     * @brief Constructor
     */
    JzOpenGLDevice();

    /**
     * @brief Destructor
     */
    ~JzOpenGLDevice() override;

    /**
     * @brief Get the Device Name
     *
     * @return String
     */
    String GetDeviceName() const override;

    /**
     * @brief Get the Vendor Name
     *
     * @return String
     */
    String GetVendorName() const override;

    /**
     * @brief Get the Driver Version
     *
     * @return String
     */
    String GetDriverVersion() const override;

    /**
     * @brief Create a Buffer
     *
     * @param desc Buffer Description
     * @return std::shared_ptr<JzRHIBuffer>
     */
    std::shared_ptr<JzRHIBuffer> CreateBuffer(const JzBufferDesc &desc) override;

    /**
     * @brief Create a Texture
     *
     * @param desc Texture Description
     * @return std::shared_ptr<JzRHITexture>
     */
    std::shared_ptr<JzRHITexture> CreateTexture(const JzTextureDesc &desc) override;

    /**
     * @brief Create a Shader
     *
     * @param desc Shader Description
     * @return std::shared_ptr<JzRHIShader>
     */
    std::shared_ptr<JzRHIShader> CreateShader(const JzShaderDesc &desc) override;

    /**
     * @brief Create a Pipeline
     *
     * @param desc Pipeline Description
     * @return std::shared_ptr<JzRHIPipeline>
     */
    std::shared_ptr<JzRHIPipeline> CreatePipeline(const JzPipelineDesc &desc) override;

    /**
     * @brief Create a Framebuffer
     *
     * @param debugName Debug Name
     * @return std::shared_ptr<JzRHIFramebuffer>
     */
    std::shared_ptr<JzRHIFramebuffer> CreateFramebuffer(const String &debugName = "") override;

    /**
     * @brief Create a Vertex Array
     *
     * @param debugName Debug Name
     * @return std::shared_ptr<JzRHIVertexArray>
     */
    std::shared_ptr<JzRHIVertexArray> CreateVertexArray(const String &debugName = "") override;

    /**
     * @brief Create a Command List
     *
     * @param debugName Debug Name
     * @return std::shared_ptr<JzRHICommandList>
     */
    std::shared_ptr<JzRHICommandList> CreateCommandList(const String &debugName = "") override;

    /**
     * @brief Execute a Command List
     *
     * @param commandList Command List
     */
    void ExecuteCommandList(std::shared_ptr<JzRHICommandList> commandList) override;

    /**
     * @brief Begin a Frame
     */
    void BeginFrame() override;

    /**
     * @brief End a Frame
     */
    void EndFrame() override;

    /**
     * @brief Present
     */
    void Present() override;

    /**
     * @brief Set Render State
     *
     * @param state Render State
     */
    void SetRenderState(const JzRenderState &state) override;

    /**
     * @brief Set Viewport
     *
     * @param viewport Viewport
     */
    void SetViewport(const JzViewport &viewport) override;

    /**
     * @brief Set Scissor
     *
     * @param scissor Scissor
     */
    void SetScissor(const JzScissorRect &scissor) override;

    /**
     * @brief Clear
     *
     * @param params Clear Parameters
     */
    void Clear(const JzClearParams &params) override;

    /**
     * @brief Draw
     *
     * @param params Draw Parameters
     */
    void Draw(const JzDrawParams &params) override;

    /**
     * @brief Draw Indexed
     *
     * @param params Draw Indexed Parameters
     */
    void DrawIndexed(const JzDrawIndexedParams &params) override;

    /**
     * @brief Bind Pipeline
     *
     * @param pipeline Pipeline
     */
    void BindPipeline(std::shared_ptr<JzRHIPipeline> pipeline) override;

    /**
     * @brief Bind Vertex Array
     *
     * @param vertexArray Vertex Array
     */
    void BindVertexArray(std::shared_ptr<JzRHIVertexArray> vertexArray) override;

    /**
     * @brief Bind Texture
     *
     * @param texture Texture
     * @param slot Slot
     */
    void BindTexture(std::shared_ptr<JzRHITexture> texture, U32 slot) override;

    /**
     * @brief Bind Framebuffer
     *
     * @param framebuffer Framebuffer
     */
    void BindFramebuffer(std::shared_ptr<JzRHIFramebuffer> framebuffer) override;

    /**
     * @brief Flush
     */
    void Flush() override;

    /**
     * @brief Finish
     */
    void Finish() override;

    /**
     * @brief Supports Multithreading
     *
     * @return Bool
     */
    Bool SupportsMultithreading() const override;

    /**
     * @brief Make Context Current
     *
     * @param threadIndex Thread Index
     */
    void MakeContextCurrent(U32 threadIndex = 0) override;

    /**
     * @brief Get Capabilities
     *
     * @return const JzRHICapabilities &
     */
    const JzRHICapabilities &GetCapabilities() const;

    /**
     * @brief Get Stats
     *
     * @return JzRHIStats &
     */
    JzRHIStats &GetStats();

private:
    void InitializeCapabilities();
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