/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Graphics/JzOpenGLDevice.h"
#include <cstdint>
#include <iostream>
#include "JzRE/Graphics/JzOpenGLBuffer.h"
#include "JzRE/Graphics/JzOpenGLTexture.h"

JzRE::JzOpenGLDevice::JzOpenGLDevice() :
    JzRE::JzDevice(JzERHIType::OpenGL)
{
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        return;
    }

    InitializeCapabilities();
}

JzRE::JzOpenGLDevice::~JzOpenGLDevice() { }

JzRE::String JzRE::JzOpenGLDevice::GetDeviceName() const
{
    const char *renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
    return renderer ? String(renderer) : "Unknown OpenGL Device";
}

JzRE::String JzRE::JzOpenGLDevice::GetVendorName() const
{
    const char *vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
    return vendor ? String(vendor) : "Unknown Vendor";
}

JzRE::String JzRE::JzOpenGLDevice::GetDriverVersion() const
{
    const char *version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
    return version ? String(version) : "Unknown Version";
}

std::shared_ptr<JzRE::JzRHIBuffer> JzRE::JzOpenGLDevice::CreateBuffer(const JzRE::JzBufferDesc &desc)
{
    auto buffer = std::make_shared<JzOpenGLBuffer>(desc);
    m_stats.buffers++;
    return buffer;
}

std::shared_ptr<JzRE::JzRHITexture> JzRE::JzOpenGLDevice::CreateTexture(const JzRE::JzTextureDesc &desc)
{
    auto texture = std::make_shared<JzOpenGLTexture>(desc);
    m_stats.textures++;
    return texture;
}

std::shared_ptr<JzRE::JzRHIShader> JzRE::JzOpenGLDevice::CreateShader(const JzRE::JzShaderDesc &desc)
{
    auto shader = std::make_shared<JzOpenGLShader>(desc);
    m_stats.shaders++;
    return shader;
}

std::shared_ptr<JzRE::JzRHIPipeline> JzRE::JzOpenGLDevice::CreatePipeline(const JzRE::JzPipelineDesc &desc)
{
    auto pipeline = std::make_shared<JzOpenGLPipeline>(desc);
    m_stats.pipelines++;
    return pipeline;
}

std::shared_ptr<JzRE::JzRHIFramebuffer> JzRE::JzOpenGLDevice::CreateFramebuffer(const JzRE::String &debugName)
{
    return std::make_shared<JzOpenGLFramebuffer>(debugName);
}

std::shared_ptr<JzRE::JzRHIVertexArray> JzRE::JzOpenGLDevice::CreateVertexArray(const JzRE::String &debugName)
{
    return std::make_shared<JzOpenGLVertexArray>(debugName);
}

std::shared_ptr<JzRE::JzRHICommandList> JzRE::JzOpenGLDevice::CreateCommandList(const JzRE::String &debugName)
{
    return std::make_shared<JzRHICommandList>(debugName);
}

void JzRE::JzOpenGLDevice::ExecuteCommandList(std::shared_ptr<JzRE::JzRHICommandList> commandList)
{
    if (commandList && !commandList->IsEmpty()) {
        commandList->Execute();
    }
}

void JzRE::JzOpenGLDevice::BeginFrame()
{
    // 重置统计信息
    m_stats.drawCalls = 0;
    m_stats.triangles = 0;
    m_stats.vertices  = 0;
}

void JzRE::JzOpenGLDevice::EndFrame()
{
    // 在帧结束时可以执行一些清理工作
    glFlush();
}

void JzRE::JzOpenGLDevice::Present()
{
    // 交换缓冲区通常由窗口系统处理
    // 这里可以执行一些额外的同步操作
    glFinish();
}

void JzRE::JzOpenGLDevice::SetRenderState(const JzRE::JzRenderState &state)
{
    m_currentRenderState = state;
    ApplyRenderState(state);
}

void JzRE::JzOpenGLDevice::SetViewport(const JzRE::JzViewport &viewport)
{
    glViewport(static_cast<GLint>(viewport.x), static_cast<GLint>(viewport.y),
               static_cast<GLsizei>(viewport.width), static_cast<GLsizei>(viewport.height));
    glDepthRange(static_cast<F64>(viewport.minDepth), static_cast<F64>(viewport.maxDepth));
}

void JzRE::JzOpenGLDevice::SetScissor(const JzRE::JzScissorRect &scissor)
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(scissor.x, scissor.y, static_cast<GLsizei>(scissor.width), static_cast<GLsizei>(scissor.height));
}

void JzRE::JzOpenGLDevice::Clear(const JzRE::JzClearParams &params)
{
    GLbitfield mask = 0;

    if (params.clearColor) {
        glClearColor(params.colorR, params.colorG, params.colorB, params.colorA);
        mask |= GL_COLOR_BUFFER_BIT;
    }

    if (params.clearDepth) {
        glClearDepth(static_cast<F64>(params.depth));
        mask |= GL_DEPTH_BUFFER_BIT;
    }

    if (params.clearStencil) {
        glClearStencil(static_cast<GLint>(params.stencil));
        mask |= GL_STENCIL_BUFFER_BIT;
    }

    if (mask != 0) {
        glClear(mask);
    }
}

void JzRE::JzOpenGLDevice::Draw(const JzRE::JzDrawParams &params)
{
    GLenum mode = ConvertPrimitiveType(params.primitiveType);

    if (params.instanceCount > 1) {
        glDrawArraysInstanced(mode,
                              static_cast<GLint>(params.firstVertex),
                              static_cast<GLsizei>(params.vertexCount),
                              static_cast<GLsizei>(params.instanceCount));
    } else {
        glDrawArrays(mode,
                     static_cast<GLint>(params.firstVertex),
                     static_cast<GLsizei>(params.vertexCount));
    }

    // 更新统计信息
    m_stats.drawCalls++;
    m_stats.vertices += params.vertexCount;
    if (params.primitiveType == JzEPrimitiveType::Triangles) {
        m_stats.triangles += params.vertexCount / 3;
    }
}

void JzRE::JzOpenGLDevice::DrawIndexed(const JzRE::JzDrawIndexedParams &params)
{
    GLenum mode = ConvertPrimitiveType(params.primitiveType);

    // 假设索引类型为 GL_UNSIGNED_INT
    const void *indices = reinterpret_cast<const void *>(params.firstIndex * sizeof(GLuint));

    if (params.instanceCount > 1) {
        glDrawElementsInstanced(mode,
                                static_cast<GLsizei>(params.indexCount),
                                GL_UNSIGNED_INT,
                                indices,
                                static_cast<GLsizei>(params.instanceCount));
    } else {
        glDrawElements(mode,
                       static_cast<GLsizei>(params.indexCount),
                       GL_UNSIGNED_INT,
                       indices);
    }

    // 更新统计信息
    m_stats.drawCalls++;
    m_stats.vertices += params.indexCount;
    if (params.primitiveType == JzEPrimitiveType::Triangles) {
        m_stats.triangles += params.indexCount / 3;
    }
}

void JzRE::JzOpenGLDevice::BindPipeline(std::shared_ptr<JzRE::JzRHIPipeline> pipeline)
{
    auto glPipeline = std::static_pointer_cast<JzOpenGLPipeline>(pipeline);
    if (glPipeline && glPipeline->IsLinked()) {
        glUseProgram(glPipeline->GetProgram());
        m_currentPipeline = glPipeline;
    }
}

void JzRE::JzOpenGLDevice::BindVertexArray(std::shared_ptr<JzRE::JzRHIVertexArray> vertexArray)
{
    auto glVertexArray = std::static_pointer_cast<JzOpenGLVertexArray>(vertexArray);
    if (glVertexArray) {
        glBindVertexArray(glVertexArray->GetHandle());
        m_currentVertexArray = glVertexArray;
    }
}

void JzRE::JzOpenGLDevice::BindTexture(std::shared_ptr<JzRE::JzRHITexture> texture, U32 slot)
{
    auto glTexture = std::static_pointer_cast<JzOpenGLTexture>(texture);
    if (glTexture) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(glTexture->GetTarget(), (GLuint)(uintptr_t)glTexture->GetTextureID());
    }
}

void JzRE::JzOpenGLDevice::BindFramebuffer(std::shared_ptr<JzRE::JzRHIFramebuffer> framebuffer)
{
    auto glFramebuffer = std::static_pointer_cast<JzOpenGLFramebuffer>(framebuffer);
    if (glFramebuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, glFramebuffer->GetHandle());
        m_currentFramebuffer = glFramebuffer;
    } else {
        // 绑定到默认帧缓冲区
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_currentFramebuffer = nullptr;
    }
}

void JzRE::JzOpenGLDevice::Flush()
{
    glFlush();
}

void JzRE::JzOpenGLDevice::Finish()
{
    glFinish();
}

JzRE::Bool JzRE::JzOpenGLDevice::SupportsMultithreading() const
{
    return false;
}

void JzRE::JzOpenGLDevice::MakeContextCurrent(JzRE::U32 threadIndex)
{
    // OpenGL 上下文切换通常由窗口系统处理
    // 这里可以执行一些必要的检查或初始化
    (void)threadIndex; // 抑制未使用参数警告
}

const JzRE::JzRHICapabilities &JzRE::JzOpenGLDevice::GetCapabilities() const
{
    return m_capabilities;
}

JzRE::JzRHIStats &JzRE::JzOpenGLDevice::GetStats()
{
    return m_stats;
}

void JzRE::JzOpenGLDevice::InitializeCapabilities()
{
    // 获取纹理支持
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, reinterpret_cast<GLint *>(&m_capabilities.maxTextureSize));
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, reinterpret_cast<GLint *>(&m_capabilities.maxTexture3DSize));
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, reinterpret_cast<GLint *>(&m_capabilities.maxTextureArrayLayers));
    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, reinterpret_cast<GLint *>(&m_capabilities.maxCubeMapTextureSize));

    // 获取渲染目标支持
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, reinterpret_cast<GLint *>(&m_capabilities.maxColorAttachments));
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, reinterpret_cast<GLint *>(&m_capabilities.maxRenderTargetSize));

    // 获取着色器支持
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, reinterpret_cast<GLint *>(&m_capabilities.maxVertexAttributes));
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, reinterpret_cast<GLint *>(&m_capabilities.maxUniformBufferBindings));
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, reinterpret_cast<GLint *>(&m_capabilities.maxTextureUnits));

    // 获取多重采样支持
    glGetIntegerv(GL_MAX_SAMPLES, reinterpret_cast<GLint *>(&m_capabilities.maxSamples));

    // 检查计算着色器支持
    m_capabilities.supportsComputeShaders = (glGetString(GL_VERSION) != nullptr);
    if (m_capabilities.supportsComputeShaders) {
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, reinterpret_cast<GLint *>(&m_capabilities.maxComputeWorkGroupSize[0]));
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, reinterpret_cast<GLint *>(&m_capabilities.maxComputeWorkGroupSize[1]));
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, reinterpret_cast<GLint *>(&m_capabilities.maxComputeWorkGroupSize[2]));
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, reinterpret_cast<GLint *>(&m_capabilities.maxComputeWorkGroupInvocations));
    }

    // 检查几何着色器支持
    m_capabilities.supportsGeometryShaders = true; // 假设支持

    // 检查细分着色器支持
    m_capabilities.supportsTessellationShaders = true; // 假设支持

    // OpenGL 不支持多线程渲染
    m_capabilities.supportsMultithreadedRendering = false;
    m_capabilities.maxRenderThreads               = 1;
}

void JzRE::JzOpenGLDevice::ApplyRenderState(const JzRE::JzRenderState &state)
{
    // 设置深度测试
    if (state.depthTest) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(ConvertDepthFunc(state.depthFunc));
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    // 设置深度写入
    glDepthMask(state.depthWrite ? GL_TRUE : GL_FALSE);

    // 设置面剪裁
    if (state.cullMode != JzECullMode::None) {
        glEnable(GL_CULL_FACE);
        glCullFace(ConvertCullMode(state.cullMode));
    } else {
        glDisable(GL_CULL_FACE);
    }

    // 设置混合模式
    if (state.blendMode != JzEBlendMode::None) {
        glEnable(GL_BLEND);
        switch (state.blendMode) {
            case JzEBlendMode::Alpha:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                break;
            case JzEBlendMode::Additive:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                break;
            case JzEBlendMode::Multiply:
                glBlendFunc(GL_DST_COLOR, GL_ZERO);
                break;
            default:
                break;
        }
    } else {
        glDisable(GL_BLEND);
    }

    // 设置线框模式
    if (state.wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

GLenum JzRE::JzOpenGLDevice::ConvertPrimitiveType(JzRE::JzEPrimitiveType type)
{
    switch (type) {
        case JzEPrimitiveType::Points: return GL_POINTS;
        case JzEPrimitiveType::Lines: return GL_LINES;
        case JzEPrimitiveType::LineStrip: return GL_LINE_STRIP;
        case JzEPrimitiveType::Triangles: return GL_TRIANGLES;
        case JzEPrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
        case JzEPrimitiveType::TriangleFan: return GL_TRIANGLE_FAN;
        default: return GL_TRIANGLES;
    }
}

GLenum JzRE::JzOpenGLDevice::ConvertBlendMode(JzRE::JzEBlendMode mode)
{
    switch (mode) {
        case JzEBlendMode::None: return GL_NONE;
        case JzEBlendMode::Alpha: return GL_SRC_ALPHA;
        case JzEBlendMode::Additive: return GL_SRC_ALPHA;
        case JzEBlendMode::Multiply: return GL_DST_COLOR;
        default: return GL_NONE;
    }
}

GLenum JzRE::JzOpenGLDevice::ConvertDepthFunc(JzRE::JzEDepthFunc func)
{
    switch (func) {
        case JzEDepthFunc::Never: return GL_NEVER;
        case JzEDepthFunc::Less: return GL_LESS;
        case JzEDepthFunc::Equal: return GL_EQUAL;
        case JzEDepthFunc::LessEqual: return GL_LEQUAL;
        case JzEDepthFunc::Greater: return GL_GREATER;
        case JzEDepthFunc::NotEqual: return GL_NOTEQUAL;
        case JzEDepthFunc::GreaterEqual: return GL_GEQUAL;
        case JzEDepthFunc::Always: return GL_ALWAYS;
        default: return GL_LESS;
    }
}

GLenum JzRE::JzOpenGLDevice::ConvertCullMode(JzRE::JzECullMode mode)
{
    switch (mode) {
        case JzECullMode::None: return GL_NONE;
        case JzECullMode::Front: return GL_FRONT;
        case JzECullMode::Back: return GL_BACK;
        case JzECullMode::FrontAndBack: return GL_FRONT_AND_BACK;
        default: return GL_BACK;
    }
}

void JzRE::JzOpenGLDevice::CheckOpenGLError(const JzRE::String &operation) const
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        String errorString;
        switch (error) {
            case GL_INVALID_ENUM: errorString = "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE: errorString = "GL_INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: errorString = "GL_INVALID_OPERATION"; break;
            case GL_OUT_OF_MEMORY: errorString = "GL_OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: errorString = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
            default: errorString = "Unknown error"; break;
        }
        std::cerr << "OpenGL error in " << operation << ": " << errorString
                  << " (0x" << std::hex << error << ")" << std::endl;
    }
}
