#include "JzDevice.h"

JzRE::JzEvent<JzRE::JzEDeviceErrorCode, JzRE::String> JzRE::JzDevice::ErrorEvent;

JzRE::JzDevice::JzDevice(const JzDeviceSettings &deviceSettings)
{
    BindErrorCallback();

    I32 initCode = glfwInit();
    if (initCode == GLFW_FALSE) {
        throw std::runtime_error("Failed to Init GLFW");
        glfwTerminate();
    } else {
        if (deviceSettings.debug) {
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, deviceSettings.contextVersionMajor);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, deviceSettings.contextVersionMinor);

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, deviceSettings.samples);

        m_isAlive = true;
    }
}

JzRE::JzDevice::~JzDevice()
{
    if (m_isAlive) {
        glfwTerminate();
    }
}

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

void JzRE::JzDevice::BindErrorCallback()
{
    auto errorCallback = [](JzRE::I32 p_code, const char *p_description) {
        ErrorEvent.Invoke(static_cast<JzRE::JzEDeviceErrorCode>(p_code), p_description);
    };

    glfwSetErrorCallback(errorCallback);
}