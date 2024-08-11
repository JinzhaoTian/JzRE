#include "GraphicsInterfaceSpotLight.h"

namespace JzRE {
GraphicsInterfaceSpotLight::GraphicsInterfaceSpotLight(const glm::vec3 &position, const glm::vec3 &direction, const glm::vec3 &color,
                                                       F32 cutOff, F32 outerCutOff, F32 constant, F32 linear, F32 quadratic) :
    GraphicsInterfaceLight(position, color), direction(direction), cutOff(cutOff), outerCutOff(outerCutOff), constant(constant),
    linear(linear), quadratic(quadratic) {
}

const glm::vec3 &GraphicsInterfaceSpotLight::GetDirection() const {
    return direction;
}

void GraphicsInterfaceSpotLight::SetDirection(const glm::vec3 &direction) {
    this->direction = direction;
}

F32 GraphicsInterfaceSpotLight::GetCutOff() const {
    return cutOff;
}

void GraphicsInterfaceSpotLight::SetCutOff(F32 cutOff) {
    this->cutOff = cutOff;
}

F32 GraphicsInterfaceSpotLight::GetOuterCutOff() const {
    return outerCutOff;
}

void GraphicsInterfaceSpotLight::SetOuterCutOff(F32 outerCutOff) {
    this->outerCutOff = outerCutOff;
}

F32 GraphicsInterfaceSpotLight::GetConstant() const {
    return constant;
}

void GraphicsInterfaceSpotLight::SetConstant(F32 constant) {
    this->constant = constant;
}

F32 GraphicsInterfaceSpotLight::GetLinear() const {
    return linear;
}

void GraphicsInterfaceSpotLight::SetLinear(F32 linear) {
    this->linear = linear;
}

F32 GraphicsInterfaceSpotLight::GetQuadratic() const {
    return quadratic;
}

void GraphicsInterfaceSpotLight::SetQuadratic(F32 quadratic) {
    this->quadratic = quadratic;
}

void GraphicsInterfaceSpotLight::ApplyLight(const GraphicsInterfaceShader &shader, I32 index) const {
    shader.SetUniform("spotLights[" + std::to_string(index) + "].position", position);
    shader.SetUniform("spotLights[" + std::to_string(index) + "].direction", direction);
    shader.SetUniform("spotLights[" + std::to_string(index) + "].color", color);
    shader.SetUniform("spotLights[" + std::to_string(index) + "].cutOff", glm::cos(glm::radians(cutOff)));
    shader.SetUniform("spotLights[" + std::to_string(index) + "].outerCutOff", glm::cos(glm::radians(outerCutOff)));
    shader.SetUniform("spotLights[" + std::to_string(index) + "].constant", constant);
    shader.SetUniform("spotLights[" + std::to_string(index) + "].linear", linear);
    shader.SetUniform("spotLights[" + std::to_string(index) + "].quadratic", quadratic);
}
}; // namespace JzRE