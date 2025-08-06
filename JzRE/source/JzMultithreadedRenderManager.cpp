#include "JzMultithreadedRenderManager.h"

JzRE::JzMultithreadedRenderManager::JzMultithreadedRenderManager() { }

JzRE::JzMultithreadedRenderManager::~JzMultithreadedRenderManager() { }

JzRE::Bool JzRE::JzMultithreadedRenderManager::Initialize(JzRE::U32 threadCount) { }

void JzRE::JzMultithreadedRenderManager::Shutdown() { }

JzRE::Bool JzRE::JzMultithreadedRenderManager::IsInitialized() const
{
    return isInitialized;
}

void JzRE::JzMultithreadedRenderManager::BeginFrame() { }

void JzRE::JzMultithreadedRenderManager::EndFrame() { }

void JzRE::JzMultithreadedRenderManager::Present() { }

void JzRE::JzMultithreadedRenderManager::SubmitCommandList(std::shared_ptr<JzRE::JzRHICommandList> commandList) { }

void JzRE::JzMultithreadedRenderManager::SubmitRenderPass(std::function<void()> renderFunc) { }

void JzRE::JzMultithreadedRenderManager::UpdateBuffer(std::shared_ptr<JzRE::JzRHIBuffer> buffer, const void *data, JzRE::Size size) { }

void JzRE::JzMultithreadedRenderManager::UpdateTexture(std::shared_ptr<JzRE::JzRHITexture> texture, const void *data) { }

void JzRE::JzMultithreadedRenderManager::SetThreadCount(JzRE::U32 threadCount) { }

JzRE::U32 JzRE::JzMultithreadedRenderManager::GetThreadCount() const { }

void JzRE::JzMultithreadedRenderManager::SetVSyncEnabled(JzRE::Bool enabled)
{
    vsyncEnabled = enabled;
}

const JzRE::JzRHIStats &JzRE::JzMultithreadedRenderManager::GetRenderStats() const { }

JzRE::F32 JzRE::JzMultithreadedRenderManager::GetAverageFrameTime() const
{
    return averageFrameTime;
}

JzRE::F32 JzRE::JzMultithreadedRenderManager::GetThreadUtilization() const { }

void JzRE::JzMultithreadedRenderManager::CreateThreadContexts(JzRE::U32 threadCount) { }

void JzRE::JzMultithreadedRenderManager::DestroyThreadContexts() { }

void JzRE::JzMultithreadedRenderManager::WaitForFrameCompletion() { }