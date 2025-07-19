#pragma once

#include "CommonTypes.h"
#include "OGLLight.h"

namespace JzRE {
class OGLSpotLight : public OGLLight {
public:
    OGLSpotLight(const glm::vec3 &position, const glm::vec3 &direction, const glm::vec3 &color,
                               F32 cutOff, F32 outerCutOff, F32 constant, F32 linear, F32 quadratic);

    const glm::vec3 &GetDirection() const;
    void SetDirection(const glm::vec3 &direction);

    F32 GetCutOff() const;
    void SetCutOff(F32 cutOff);

    F32 GetOuterCutOff() const;
    void SetOuterCutOff(F32 outerCutOff);

    F32 GetConstant() const;
    void SetConstant(F32 constant);

    F32 GetLinear() const;
    void SetLinear(F32 linear);

    F32 GetQuadratic() const;
    void SetQuadratic(F32 quadratic);

    void ApplyLight(std::shared_ptr<OGLShader> shader, I32 index) const override;

private:
    glm::vec3 direction;
    F32 cutOff;
    F32 outerCutOff;
    F32 constant;
    F32 linear;
    F32 quadratic;
};
} // namespace JzRE