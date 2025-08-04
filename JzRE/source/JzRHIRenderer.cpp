#include "JzRHIRenderer.h"

JzRE::JzRHIRenderer::JzRHIRenderer(std::shared_ptr<JzRE::OGLRenderWindow> wnd, I32 width, I32 height) { }

JzRE::JzRHIRenderer::~JzRHIRenderer() { }

void JzRE::JzRHIRenderer::RenderScene(std::shared_ptr<JzRE::JzScene> scene) { }

JzRE::Bool JzRE::JzRHIRenderer::Initialize()
{
    return false;
}

void JzRE::JzRHIRenderer::Shutdown() { }

void JzRE::JzRHIRenderer::SetUseCommandBuffer(JzRE::Bool useCommandBuffer)
{
    this->useCommandBuffer = useCommandBuffer;
}

JzRE::Bool JzRE::JzRHIRenderer::IsUsingCommandBuffer() const
{
    return useCommandBuffer;
}

void JzRE::JzRHIRenderer::SetThreadCount(JzRE::U32 threadCount) { }

JzRE::U32 JzRE::JzRHIRenderer::GetThreadCount() const
{
    return 1;
}

const JzRE::JzRHIStats &JzRE::JzRHIRenderer::GetStats() const { }

void JzRE::JzRHIRenderer::ResetStats() { }

JzRE::Bool JzRE::JzRHIRenderer::CreateFramebuffer()
{
    return false;
}

JzRE::Bool JzRE::JzRHIRenderer::CreateDefaultPipeline()
{
    return false;
}

void JzRE::JzRHIRenderer::CleanupResources() { }

void JzRE::JzRHIRenderer::RenderImmediate(std::shared_ptr<JzRE::JzScene> scene) { }

void JzRE::JzRHIRenderer::RenderWithCommandBuffer(std::shared_ptr<JzRE::JzScene> scene) { }

void JzRE::JzRHIRenderer::SetupViewport() { }

void JzRE::JzRHIRenderer::ClearBuffers() { }