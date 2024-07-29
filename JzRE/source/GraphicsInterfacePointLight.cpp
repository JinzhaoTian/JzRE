#include "GraphicsInterfacePointLight.h"

namespace JzRE {
GraphicsInterfacePointLight::GraphicsInterfacePointLight(const glm::vec3 &position, const glm::vec3 &color, F32 constant, F32 linear, F32 quadratic) :
    GraphicsInterfaceLight(position, color), constant(constant), linear(linear), quadratic(quadratic) {
}

F32 GraphicsInterfacePointLight::GetConstant() const {
    return constant;
}

void GraphicsInterfacePointLight::SetConstant(F32 constant) {
    this->constant = constant;
}

F32 GraphicsInterfacePointLight::GetLinear() const {
    return linear;
}

void GraphicsInterfacePointLight::SetLinear(F32 linear) {
    this->linear = linear;
}

F32 GraphicsInterfacePointLight::GetQuadratic() const {
    return quadratic;
}

void GraphicsInterfacePointLight::SetQuadratic(F32 quadratic) {
    this->quadratic = quadratic;
}

void GraphicsInterfacePointLight::ApplyLight(SharedPtr<GraphicsInterfaceShader> shader, I32 index) const {
    shader->SetUniform("pointLights[" + std::to_string(index) + "].position", position);
    shader->SetUniform("pointLights[" + std::to_string(index) + "].color", color);
    shader->SetUniform("pointLights[" + std::to_string(index) + "].constant", constant);
    shader->SetUniform("pointLights[" + std::to_string(index) + "].linear", linear);
    shader->SetUniform("pointLights[" + std::to_string(index) + "].quadratic", quadratic);
}
} // namespace JzRE