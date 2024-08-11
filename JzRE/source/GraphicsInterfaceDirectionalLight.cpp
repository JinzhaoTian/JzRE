#include "GraphicsInterfaceDirectionalLight.h"

namespace JzRE {
GraphicsInterfaceDirectionalLight::GraphicsInterfaceDirectionalLight(const glm::vec3 &direction, const glm::vec3 &color) :
    GraphicsInterfaceLight(glm::vec3(0.0f), color), direction(direction) {
}

const glm::vec3 &GraphicsInterfaceDirectionalLight::GetDirection() const {
    return direction;
}

void GraphicsInterfaceDirectionalLight::SetDirection(const glm::vec3 &direction) {
    this->direction = direction;
}

void GraphicsInterfaceDirectionalLight::ApplyLight(const GraphicsInterfaceShader &shader, I32 index) const {
    shader.SetUniform("directionalLight[" + std::to_string(index) + "].direction", direction);
    shader.SetUniform("directionalLight[" + std::to_string(index) + "].color", color);
}
}; // namespace JzRE