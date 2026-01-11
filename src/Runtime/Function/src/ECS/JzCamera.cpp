/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzCamera.h"

JzRE::JzCamera::JzCamera() :
    m_projectionMode(JzEProjectionMode::PERSPECTIVE),
    m_fov(60.0f),
    m_size(5.0f),
    m_near(0.1f),
    m_far(1000.f),
    m_clearColor(0.f, 0.f, 0.f) { }

void JzRE::JzCamera::SetPosition(const JzRE::JzVec3 &position)
{
    m_position = position;
}

const JzRE::JzVec3 &JzRE::JzCamera::GetPosition() const
{
    return m_position;
}

void JzRE::JzCamera::SetRotation(const JzRE::JzVec4 &rotation)
{
    m_rotation = rotation;
}

const JzRE::JzVec4 &JzRE::JzCamera::GetRotation() const
{
    return m_rotation;
}

void JzRE::JzCamera::SetSize(JzRE::F32 value)
{
    m_size = value;
}

JzRE::F32 JzRE::JzCamera::GetSize() const
{
    return m_size;
}

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

void JzRE::JzCamera::SetNear(JzRE::F32 value)
{
    m_near = value;
}

JzRE::F32 JzRE::JzCamera::GetNear() const
{
    return m_far;
}

void JzRE::JzCamera::SetClearColor(const JzVec3 &clearColor)
{
    m_clearColor = clearColor;
}

const JzRE::JzVec3 &JzRE::JzCamera::GetClearColor() const
{
    return m_clearColor;
}

const JzRE::JzMat4 &JzRE::JzCamera::GetProjectionMatrix() const
{
    return m_projectionMatrix;
}

const JzRE::JzMat4 &JzRE::JzCamera::GetViewMatrix() const
{
    return m_viewMatrix; // TODO calculate
}

void JzRE::JzCamera::SetProjectionMode(JzRE::JzEProjectionMode projectionMode)
{
    m_projectionMode = projectionMode;
}

JzRE::JzEProjectionMode JzRE::JzCamera::GetProjectionMode() const
{
    return m_projectionMode;
}