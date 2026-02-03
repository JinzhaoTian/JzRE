/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Platform/RHI/JzGraphicsContext.h"

#include "JzRE/Runtime/Platform/RHI/JzDeviceFactory.h"

namespace JzRE {

JzGraphicsContext::~JzGraphicsContext()
{
    Shutdown();
}

void JzGraphicsContext::Initialize(JzIWindowBackend &windowBackend, JzERHIType rhiType)
{
    m_windowBackend = &windowBackend;
    m_rhiType       = rhiType;

    // Ensure the window context is current before creating the device.
    m_windowBackend->MakeContextCurrent();

    m_device = JzDeviceFactory::CreateDevice(m_rhiType);
}

void JzGraphicsContext::Shutdown()
{
    if (m_device) {
        m_device.reset();
    }
    m_windowBackend = nullptr;
    m_rhiType       = JzERHIType::Unknown;
}

JzDevice &JzGraphicsContext::GetDevice()
{
    return *m_device;
}

const JzDevice &JzGraphicsContext::GetDevice() const
{
    return *m_device;
}

void JzGraphicsContext::MakeCurrentContext(U32 threadIndex)
{
    if (!m_windowBackend) {
        return;
    }

    (void)threadIndex;
    m_windowBackend->MakeContextCurrent();
}

void JzGraphicsContext::BeginFrame()
{
    if (!m_device) {
        return;
    }

    m_device->BeginFrame();
}

void JzGraphicsContext::EndFrame()
{
    if (!m_device) {
        return;
    }

    m_device->EndFrame();
}

void JzGraphicsContext::Present()
{
    if (!m_windowBackend || !m_device) {
        return;
    }

    // Ensure GPU work is finished before swapping.
    m_device->Finish();
    m_windowBackend->SwapBuffers();
}

Bool JzGraphicsContext::IsInitialized() const
{
    return m_device != nullptr;
}

} // namespace JzRE
