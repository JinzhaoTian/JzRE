#include "OGLDirectionalLight.h"

namespace JzRE {
OGLDirectionalLight::OGLDirectionalLight(const glm::vec3 &direction, const glm::vec3 &color) :
    OGLLight(glm::vec3(0.0f), color), direction(direction) {
}

const glm::vec3 &OGLDirectionalLight::GetDirection() const {
    return direction;
}

void OGLDirectionalLight::SetDirection(const glm::vec3 &direction) {
    this->direction = direction;
}

void OGLDirectionalLight::ApplyLight(std::shared_ptr<OGLShader> shader, I32 index) const {
    shader->SetUniform("directionalLight[" + std::to_string(index) + "].direction", direction);
    shader->SetUniform("directionalLight[" + std::to_string(index) + "].color", color);
}
}; // namespace JzRE