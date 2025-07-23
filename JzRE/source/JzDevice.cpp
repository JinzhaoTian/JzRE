#include "JzDevice.h"

JzRE::JzDevice::JzDevice(const JzDeviceSettings &deviceSettings) { }

JzRE::JzDevice::~JzDevice() { }

JzRE::Bool JzRE::JzDevice::IsVsync() const
{
    return m_isVsync;
}

void JzRE::JzDevice::SetVsync(JzRE::Bool value)
{
    glfwSwapInterval(value ? 1 : 0);
    m_isVsync = value;
}

void JzRE::JzDevice::PollEvents() const
{
    glfwPollEvents();
}

JzRE::F32 JzRE::JzDevice::GetElapsedTime() const
{
    return static_cast<JzRE::F32>(glfwGetTime());
}
