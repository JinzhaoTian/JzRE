#include "JzRHIRenderer.h"

JzRE::JzRHIRenderer::JzRHIRenderer() { }

JzRE::JzRHIRenderer::~JzRHIRenderer() { }

void JzRE::JzRHIRenderer::RenderScene(JzRE::JzScene *scene)
{
    for (const auto model : scene->GetModels()) {
        // model->Draw();
    }
}

void JzRE::JzRHIRenderer::SetUseCommandList(JzRE::Bool useCommandList)
{
    m_useCommandList = useCommandList;
}

JzRE::Bool JzRE::JzRHIRenderer::IsUsingCommandList() const
{
    return m_useCommandList;
}

void JzRE::JzRHIRenderer::SetThreadCount(JzRE::U32 threadCount) { }

JzRE::U32 JzRE::JzRHIRenderer::GetThreadCount() const
{
    return 1;
}

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

void JzRE::JzRHIRenderer::RenderWithCommandList(std::shared_ptr<JzRE::JzScene> scene) { }

void JzRE::JzRHIRenderer::SetupViewport() { }

void JzRE::JzRHIRenderer::ClearBuffers() { }