#pragma once

#include "RHI.h"
#include "CommonTypes.h"

namespace JzRE {

/**
 * OpenGL缓冲区实现
 */
class OpenGLBuffer : public RHIBuffer {
public:
    OpenGLBuffer(const BufferDesc& desc);
    ~OpenGLBuffer() override;

    void UpdateData(const void* data, Size size, Size offset = 0) override;
    void* MapBuffer() override;
    void UnmapBuffer() override;

    GLuint GetHandle() const { return handle; }

private:
    GLuint handle = 0;
    GLenum target;
    GLenum usage;
};

/**
 * OpenGL纹理实现
 */
class OpenGLTexture : public RHITexture {
public:
    OpenGLTexture(const TextureDesc& desc);
    ~OpenGLTexture() override;

    void UpdateData(const void* data, U32 mipLevel = 0, U32 arrayIndex = 0) override;
    void GenerateMipmaps() override;

    GLuint GetHandle() const { return handle; }
    GLenum GetTarget() const { return target; }

private:
    GLuint handle = 0;
    GLenum target;
    GLenum internalFormat;
    GLenum format;
    GLenum type;

    static GLenum ConvertTextureType(ETextureType type);
    static GLenum ConvertInternalFormat(ETextureFormat format);
    static GLenum ConvertFormat(ETextureFormat format);
    static GLenum ConvertType(ETextureFormat format);
    static GLenum ConvertFilter(ETextureFilter filter);
    static GLenum ConvertWrap(ETextureWrap wrap);
};

/**
 * OpenGL着色器实现
 */
class OpenGLShader : public RHIShader {
public:
    OpenGLShader(const ShaderDesc& desc);
    ~OpenGLShader() override;

    GLuint GetHandle() const { return handle; }
    Bool IsCompiled() const { return isCompiled; }
    const String& GetCompileLog() const { return compileLog; }

private:
    GLuint handle = 0;
    Bool isCompiled = false;
    String compileLog;

    static GLenum ConvertShaderType(EShaderType type);
    Bool CompileShader();
};

/**
 * OpenGL渲染管线实现
 */
class OpenGLPipeline : public RHIPipeline {
public:
    OpenGLPipeline(const PipelineDesc& desc);
    ~OpenGLPipeline() override;

    GLuint GetProgram() const { return program; }
    Bool IsLinked() const { return isLinked; }
    const String& GetLinkLog() const { return linkLog; }

    // Uniform设置接口
    void SetUniform(const String& name, I32 value);
    void SetUniform(const String& name, F32 value);
    void SetUniform(const String& name, const glm::vec2& value);
    void SetUniform(const String& name, const glm::vec3& value);
    void SetUniform(const String& name, const glm::vec4& value);
    void SetUniform(const String& name, const glm::mat3& value);
    void SetUniform(const String& name, const glm::mat4& value);

private:
    GLuint program = 0;
    Bool isLinked = false;
    String linkLog;
    std::vector<std::shared_ptr<OpenGLShader>> shaders;
    std::unordered_map<String, GLint> uniformLocations;

    Bool LinkProgram();
    GLint GetUniformLocation(const String& name);
};

/**
 * OpenGL帧缓冲实现
 */
class OpenGLFramebuffer : public RHIFramebuffer {
public:
    OpenGLFramebuffer(const String& debugName = "");
    ~OpenGLFramebuffer() override;

    void AttachColorTexture(std::shared_ptr<RHITexture> texture, U32 attachmentIndex = 0) override;
    void AttachDepthTexture(std::shared_ptr<RHITexture> texture) override;
    void AttachDepthStencilTexture(std::shared_ptr<RHITexture> texture) override;
    Bool IsComplete() const override;

    GLuint GetHandle() const { return handle; }

private:
    GLuint handle = 0;
    std::vector<std::shared_ptr<RHITexture>> colorAttachments;
    std::shared_ptr<RHITexture> depthAttachment;
    std::shared_ptr<RHITexture> depthStencilAttachment;
};

/**
 * OpenGL顶点数组对象实现
 */
class OpenGLVertexArray : public RHIVertexArray {
public:
    OpenGLVertexArray(const String& debugName = "");
    ~OpenGLVertexArray() override;

    void BindVertexBuffer(std::shared_ptr<RHIBuffer> buffer, U32 binding = 0) override;
    void BindIndexBuffer(std::shared_ptr<RHIBuffer> buffer) override;
    void SetVertexAttribute(U32 index, U32 size, U32 stride, U32 offset) override;

    GLuint GetHandle() const { return handle; }

private:
    GLuint handle = 0;
    std::vector<std::shared_ptr<RHIBuffer>> vertexBuffers;
    std::shared_ptr<RHIBuffer> indexBuffer;
};

/**
 * OpenGL设备实现
 */
class OpenGLDevice : public RHIDevice {
public:
    OpenGLDevice();
    ~OpenGLDevice() override;

    // 设备信息
    String GetDeviceName() const override;
    String GetVendorName() const override;
    String GetDriverVersion() const override;

    // 资源创建接口
    std::shared_ptr<RHIBuffer> CreateBuffer(const BufferDesc& desc) override;
    std::shared_ptr<RHITexture> CreateTexture(const TextureDesc& desc) override;
    std::shared_ptr<RHIShader> CreateShader(const ShaderDesc& desc) override;
    std::shared_ptr<RHIPipeline> CreatePipeline(const PipelineDesc& desc) override;
    std::shared_ptr<RHIFramebuffer> CreateFramebuffer(const String& debugName = "") override;
    std::shared_ptr<RHIVertexArray> CreateVertexArray(const String& debugName = "") override;

    // 命令系统
    std::shared_ptr<RHICommandBuffer> CreateCommandBuffer(const String& debugName = "") override;
    void ExecuteCommandBuffer(std::shared_ptr<RHICommandBuffer> commandBuffer) override;

    // 立即渲染模式
    void BeginFrame() override;
    void EndFrame() override;
    void Present() override;

    // 渲染状态管理
    void SetRenderState(const RenderState& state) override;
    void SetViewport(const Viewport& viewport) override;
    void SetScissor(const ScissorRect& scissor) override;

    // 绘制接口
    void Clear(const ClearParams& params) override;
    void Draw(const DrawParams& params) override;
    void DrawIndexed(const DrawIndexedParams& params) override;

    // 资源绑定
    void BindPipeline(std::shared_ptr<RHIPipeline> pipeline) override;
    void BindVertexArray(std::shared_ptr<RHIVertexArray> vertexArray) override;
    void BindTexture(std::shared_ptr<RHITexture> texture, U32 slot) override;
    void BindFramebuffer(std::shared_ptr<RHIFramebuffer> framebuffer) override;

    // 同步和等待
    void Flush() override;
    void Finish() override;

    // 多线程支持
    Bool SupportsMultithreading() const override;
    void MakeContextCurrent(U32 threadIndex = 0) override;

    // OpenGL特定功能
    const RHICapabilities& GetCapabilities() const { return capabilities; }
    RHIStats& GetStats() { return stats; }

private:
    RHICapabilities capabilities;
    RHIStats stats;
    RenderState currentRenderState;
    
    std::shared_ptr<OpenGLPipeline> currentPipeline;
    std::shared_ptr<OpenGLVertexArray> currentVertexArray;
    std::shared_ptr<OpenGLFramebuffer> currentFramebuffer;

    void InitializeCapabilities();
    void ApplyRenderState(const RenderState& state);
    
    static GLenum ConvertPrimitiveType(EPrimitiveType type);
    static GLenum ConvertBlendMode(EBlendMode mode);
    static GLenum ConvertDepthFunc(EDepthFunc func);
    static GLenum ConvertCullMode(ECullMode mode);
};

} // namespace JzRE 