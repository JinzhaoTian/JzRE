/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzLight.h"

JzRE::JzLight::JzLight(const JzRE::JzVec3 &position, const JzRE::JzVec3 &color) :
    position(position),
    color(color) { }

void JzRE::JzLight::SetPosition(const JzRE::JzVec3 &position)
{
    this->position = position;
}

const JzRE::JzVec3 &JzRE::JzLight::GetPosition() const
{
    return this->position;
}

void JzRE::JzLight::SetColor(const JzRE::JzVec3 &color)
{
    this->color = color;
}

const JzRE::JzVec3 &JzRE::JzLight::GetColor() const
{
    return this->color;
}
