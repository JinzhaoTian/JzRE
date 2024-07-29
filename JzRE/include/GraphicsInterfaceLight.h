#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceShader.h"

namespace JzRE {
class GraphicsInterfaceLight {
public:
public:
    GraphicsInterfaceLight(const glm::vec3 &position, const glm::vec3 &color);
    virtual ~GraphicsInterfaceLight() = default;

    void SetPosition(const glm::vec3 &position);
    const glm::vec3 &GetPosition() const;

    void SetColor(const glm::vec3 &color);
    const glm::vec3 &GetColor() const;

    virtual void ApplyLight(SharedPtr<GraphicsInterfaceShader> shader, int index) const = 0;

protected:
    glm::vec3 position;
    glm::vec3 color;
};
}; // namespace JzRE