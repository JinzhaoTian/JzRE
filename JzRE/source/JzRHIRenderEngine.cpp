#include "JzRHIRenderEngine.h"

JzRE::JzRHIRenderEngine::JzRHIRenderEngine() { }

JzRE::JzRHIRenderEngine::~JzRHIRenderEngine() { }

JzRE::Bool JzRE::JzRHIRenderEngine::Initialize(JzRE::JzERHIType rhiType)
{
    return false;
}

void JzRE::JzRHIRenderEngine::Run() { }

void JzRE::JzRHIRenderEngine::Shutdown() { }

void JzRE::JzRHIRenderEngine::SetUseCommandBuffer(JzRE::Bool useCommandBuffer) { }

void JzRE::JzRHIRenderEngine::SetThreadCount(JzRE::U32 threadCount) { }

JzRE::Bool JzRE::JzRHIRenderEngine::InitScene()
{
    return false;
}

void JzRE::JzRHIRenderEngine::ProcessInput() { }

void JzRE::JzRHIRenderEngine::UpdateStats() { }