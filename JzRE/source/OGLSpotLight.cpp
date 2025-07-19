#include "OGLSpotLight.h"

namespace JzRE {
OGLSpotLight::OGLSpotLight(const glm::vec3 &position, const glm::vec3 &direction, const glm::vec3 &color,
                                                       F32 cutOff, F32 outerCutOff, F32 constant, F32 linear, F32 quadratic) :
    OGLLight(position, color), direction(direction), cutOff(cutOff), outerCutOff(outerCutOff), constant(constant),
    linear(linear), quadratic(quadratic) {
}

const glm::vec3 &OGLSpotLight::GetDirection() const {
    return direction;
}

void OGLSpotLight::SetDirection(const glm::vec3 &direction) {
    this->direction = direction;
}

F32 OGLSpotLight::GetCutOff() const {
    return cutOff;
}

void OGLSpotLight::SetCutOff(F32 cutOff) {
    this->cutOff = cutOff;
}

F32 OGLSpotLight::GetOuterCutOff() const {
    return outerCutOff;
}

void OGLSpotLight::SetOuterCutOff(F32 outerCutOff) {
    this->outerCutOff = outerCutOff;
}

F32 OGLSpotLight::GetConstant() const {
    return constant;
}

void OGLSpotLight::SetConstant(F32 constant) {
    this->constant = constant;
}

F32 OGLSpotLight::GetLinear() const {
    return linear;
}

void OGLSpotLight::SetLinear(F32 linear) {
    this->linear = linear;
}

F32 OGLSpotLight::GetQuadratic() const {
    return quadratic;
}

void OGLSpotLight::SetQuadratic(F32 quadratic) {
    this->quadratic = quadratic;
}

void OGLSpotLight::ApplyLight(std::shared_ptr<OGLShader> shader, I32 index) const {
    shader->SetUniform("spotLights[" + std::to_string(index) + "].position", position);
    shader->SetUniform("spotLights[" + std::to_string(index) + "].direction", direction);
    shader->SetUniform("spotLights[" + std::to_string(index) + "].color", color);
    shader->SetUniform("spotLights[" + std::to_string(index) + "].cutOff", glm::cos(glm::radians(cutOff)));
    shader->SetUniform("spotLights[" + std::to_string(index) + "].outerCutOff", glm::cos(glm::radians(outerCutOff)));
    shader->SetUniform("spotLights[" + std::to_string(index) + "].constant", constant);
    shader->SetUniform("spotLights[" + std::to_string(index) + "].linear", linear);
    shader->SetUniform("spotLights[" + std::to_string(index) + "].quadratic", quadratic);
}
}; // namespace JzRE