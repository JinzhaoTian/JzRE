#include "OGLPointLight.h"

namespace JzRE {
OGLPointLight::OGLPointLight(const glm::vec3 &position, const glm::vec3 &color, F32 constant, F32 linear, F32 quadratic) :
    OGLLight(position, color), constant(constant), linear(linear), quadratic(quadratic) {
}

F32 OGLPointLight::GetConstant() const {
    return constant;
}

void OGLPointLight::SetConstant(F32 constant) {
    this->constant = constant;
}

F32 OGLPointLight::GetLinear() const {
    return linear;
}

void OGLPointLight::SetLinear(F32 linear) {
    this->linear = linear;
}

F32 OGLPointLight::GetQuadratic() const {
    return quadratic;
}

void OGLPointLight::SetQuadratic(F32 quadratic) {
    this->quadratic = quadratic;
}

void OGLPointLight::ApplyLight(SharedPtr<OGLShader> shader, I32 index) const {
    shader->SetUniform("pointLights[" + std::to_string(index) + "].position", position);
    shader->SetUniform("pointLights[" + std::to_string(index) + "].color", color);
    shader->SetUniform("pointLights[" + std::to_string(index) + "].constant", constant);
    shader->SetUniform("pointLights[" + std::to_string(index) + "].linear", linear);
    shader->SetUniform("pointLights[" + std::to_string(index) + "].quadratic", quadratic);
}
} // namespace JzRE