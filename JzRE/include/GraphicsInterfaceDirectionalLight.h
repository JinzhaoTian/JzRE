#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceLight.h"

namespace JzRE {
class GraphicsInterfaceDirectionalLight : public GraphicsInterfaceLight {
public:
    GraphicsInterfaceDirectionalLight(const glm::vec3 &direction, const glm::vec3 &color);

    const glm::vec3 &GetDirection() const;
    void SetDirection(const glm::vec3 &direction);

    void ApplyLight(SharedPtr<GraphicsInterfaceShader> shader, I32 index) const override;

private:
    glm::vec3 direction;
};
} // namespace JzRE