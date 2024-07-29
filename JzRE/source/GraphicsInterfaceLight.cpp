#include "GraphicsInterfaceLight.h"

namespace JzRE {
GraphicsInterfaceLight::GraphicsInterfaceLight(const glm::vec3 &position, const glm::vec3 &color) :
    position(position), color(color) {
}

void GraphicsInterfaceLight::SetPosition(const glm::vec3 &position) {
    this->position = position;
}

const glm::vec3 &GraphicsInterfaceLight::GetPosition() const {
    return this->position;
}

void GraphicsInterfaceLight::SetColor(const glm::vec3 &color) {
    this->color = color;
}

const glm::vec3 &GraphicsInterfaceLight::GetColor() const {
    return this->color;
}

} // namespace JzRE