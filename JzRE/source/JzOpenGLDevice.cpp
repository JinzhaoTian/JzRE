#include "JzOpenGLDevice.h"

JzRE::JzOpenGLDevice::JzOpenGLDevice() :
    JzRE::JzRHIDevice(JzERHIType::OpenGL) { }

JzRE::JzOpenGLDevice::~JzOpenGLDevice() { }

JzRE::String JzRE::JzOpenGLDevice::GetDeviceName() const
{
    return "";
}

JzRE::String JzRE::JzOpenGLDevice::GetVendorName() const
{
    return "";
}

JzRE::String JzRE::JzOpenGLDevice::GetDriverVersion() const
{
    return "";
}

std::shared_ptr<JzRE::JzRHIBuffer> JzRE::JzOpenGLDevice::CreateBuffer(const JzRE::JzBufferDesc &desc) { }

std::shared_ptr<JzRE::JzRHITexture> JzRE::JzOpenGLDevice::CreateTexture(const JzRE::JzTextureDesc &desc) { }

std::shared_ptr<JzRE::JzRHIShader> JzRE::JzOpenGLDevice::CreateShader(const JzRE::JzShaderDesc &desc) { }

std::shared_ptr<JzRE::JzRHIPipeline> JzRE::JzOpenGLDevice::CreatePipeline(const JzRE::JzPipelineDesc &desc) { }

std::shared_ptr<JzRE::JzRHIFramebuffer> JzRE::JzOpenGLDevice::CreateFramebuffer(const JzRE::String &debugName) { }

std::shared_ptr<JzRE::JzRHIVertexArray> JzRE::JzOpenGLDevice::CreateVertexArray(const JzRE::String &debugName) { }

std::shared_ptr<JzRE::JzRHICommandBuffer> JzRE::JzOpenGLDevice::CreateCommandBuffer(const JzRE::String &debugName) { }

void JzRE::JzOpenGLDevice::ExecuteCommandBuffer(std::shared_ptr<JzRE::JzRHICommandBuffer> commandBuffer) { }

void JzRE::JzOpenGLDevice::BeginFrame() { }

void JzRE::JzOpenGLDevice::EndFrame() { }

void JzRE::JzOpenGLDevice::Present() { }

void JzRE::JzOpenGLDevice::SetRenderState(const JzRE::JzRenderState &state) { }

void JzRE::JzOpenGLDevice::SetViewport(const JzRE::JzViewport &viewport) { }

void JzRE::JzOpenGLDevice::SetScissor(const JzRE::JzScissorRect &scissor) { }

void JzRE::JzOpenGLDevice::Clear(const JzRE::JzClearParams &params) { }

void JzRE::JzOpenGLDevice::Draw(const JzRE::JzDrawParams &params) { }

void JzRE::JzOpenGLDevice::DrawIndexed(const JzRE::JzDrawIndexedParams &params) { }

void JzRE::JzOpenGLDevice::BindPipeline(std::shared_ptr<JzRE::JzRHIPipeline> pipeline) { }

void JzRE::JzOpenGLDevice::BindVertexArray(std::shared_ptr<JzRE::JzRHIVertexArray> vertexArray) { }

void JzRE::JzOpenGLDevice::BindTexture(std::shared_ptr<JzRE::JzRHITexture> texture, U32 slot) { }

void JzRE::JzOpenGLDevice::BindFramebuffer(std::shared_ptr<JzRE::JzRHIFramebuffer> framebuffer) { }

void JzRE::JzOpenGLDevice::Flush() { }

void JzRE::JzOpenGLDevice::Finish() { }

JzRE::Bool JzRE::JzOpenGLDevice::SupportsMultithreading() const
{
    return false;
}

void JzRE::JzOpenGLDevice::MakeContextCurrent(JzRE::U32 threadIndex) { }

const JzRE::JzRHICapabilities &JzRE::JzOpenGLDevice::GetCapabilities() const
{
    return capabilities;
}

JzRE::JzRHIStats &JzRE::JzOpenGLDevice::GetStats()
{
    return stats;
}

void JzRE::JzOpenGLDevice::InitializeCapabilities() { }

void JzRE::JzOpenGLDevice::ApplyRenderState(const JzRE::JzRenderState &state) { }

GLenum JzRE::JzOpenGLDevice::ConvertPrimitiveType(JzRE::JzEPrimitiveType type) { }

GLenum JzRE::JzOpenGLDevice::ConvertBlendMode(JzRE::JzEBlendMode mode) { }

GLenum JzRE::JzOpenGLDevice::ConvertDepthFunc(JzRE::JzEDepthFunc func) { }

GLenum JzRE::JzOpenGLDevice::ConvertCullMode(JzRE::JzECullMode mode) { }
