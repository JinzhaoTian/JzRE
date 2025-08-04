#include "JzContext.h"

JzRE::JzContext::JzContext(JzERHIType rhiType) :
    sceneManager()
{
    /* Device */
    auto devicePtr = JzRHIFactory::CreateDevice(rhiType);
    if (!devicePtr) {
        return;
    }

    m_device = std::shared_ptr<JzRHIDevice>(devicePtr.release());

    m_commandQueue = std::make_shared<JzRHICommandQueue>();

    device = std::make_unique<JzRE::JzDevice>(deviceSettings);

    /* Window */
    window = std::make_unique<JzRE::JzWindow>(*device, windowSettings);

    /* Input Manager */
    inputManager = std::make_unique<JzRE::JzInputManager>(*window);

    window->MakeCurrentContext();

    device->SetVsync(true);

    /* UI Manager */
    uiManager = std::make_unique<JzRE::JzUIManager>(window->GetGLFWWindow());
    uiManager->SetDocking(true);

    /* Service Providing */
    JzServiceContainer::Provide<JzWindow>(*window);
    JzServiceContainer::Provide<JzInputManager>(*inputManager);
    JzServiceContainer::Provide<JzSceneManager>(sceneManager);
}

JzRE::JzContext::~JzContext()
{
    if (m_commandQueue) {
        m_commandQueue.reset();
    }

    if (m_device) {
        m_device.reset();
    }
}

std::shared_ptr<JzRE::JzRHIDevice> JzRE::JzContext::GetDevice() const
{
    return m_device;
}

JzRE::JzERHIType JzRE::JzContext::GetRHIType() const
{
    return m_device ? m_device->GetRHIType() : JzERHIType::Unknown;
}

std::shared_ptr<JzRE::JzRHICommandQueue> JzRE::JzContext::GetCommandQueue() const
{
    return m_commandQueue;
}

void JzRE::JzContext::SetThreadCount(JzRE::U32 threadCount)
{
    if (m_commandQueue) {
        m_commandQueue->SetThreadCount(threadCount);
    }
}

JzRE::U32 JzRE::JzContext::GetThreadCount() const
{
    if (m_commandQueue) {
        return m_commandQueue->GetThreadCount();
    }
    return 1;
}