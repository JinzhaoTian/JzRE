/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzRenderThreadContext.h"

JzRE::JzRenderThreadContext::JzRenderThreadContext(JzRE::U32 threadId) { }

JzRE::JzRenderThreadContext::~JzRenderThreadContext() { }

JzRE::Bool JzRE::JzRenderThreadContext::Initialize()
{
    return false;
}

void JzRE::JzRenderThreadContext::MakeCurrent() { }

void JzRE::JzRenderThreadContext::SwapBuffers() { }

void JzRE::JzRenderThreadContext::Cleanup() { }

JzRE::U32 JzRE::JzRenderThreadContext::GetThreadId() const
{
    return threadId;
}

JzRE::Bool JzRE::JzRenderThreadContext::IsInitialized() const
{
    return isInitialized;
}