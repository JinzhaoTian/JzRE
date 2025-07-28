#include "JzCamera.h"

JzRE::JzCamera::JzCamera() { }

void JzRE::JzCamera::SetFov(JzRE::F32 value)
{
    m_fov = value;
}

JzRE::F32 JzRE::JzCamera::GetFov() const
{
    return m_fov;
}

void JzRE::JzCamera::SetFar(JzRE::F32 value)
{
    m_far = value;
}

JzRE::F32 JzRE::JzCamera::GetFar() const
{
    return m_far;
}