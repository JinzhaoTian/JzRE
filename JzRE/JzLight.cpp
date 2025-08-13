#include "JzLight.h"

namespace JzRE {
JzLight::JzLight(const glm::vec3 &position, const glm::vec3 &color) :
    position(position), color(color)
{
}

void JzLight::SetPosition(const glm::vec3 &position)
{
    this->position = position;
}

const glm::vec3 &JzLight::GetPosition() const
{
    return this->position;
}

void JzLight::SetColor(const glm::vec3 &color)
{
    this->color = color;
}

const glm::vec3 &JzLight::GetColor() const
{
    return this->color;
}

} // namespace JzRE