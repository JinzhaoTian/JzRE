#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceLight.h"

namespace JzRE {
class GraphicsInterfacePointLight : public GraphicsInterfaceLight {
public:
    GraphicsInterfacePointLight(const glm::vec3 &position, const glm::vec3 &color, F32 constant, F32 linear, F32 quadratic);

    F32 GetConstant() const;
    void SetConstant(F32 constant);

    F32 GetLinear() const;
    void SetLinear(F32 linear);

    F32 GetQuadratic() const;
    void SetQuadratic(F32 quadratic);

    void ApplyLight(const GraphicsInterfaceShader &shader, I32 index) const override;

private:
    F32 constant;
    F32 linear;
    F32 quadratic;
};
} // namespace JzRE