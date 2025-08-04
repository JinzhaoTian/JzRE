#pragma once

#include "CommonTypes.h"
#include "OGLShader.h"

namespace JzRE {
class JzLight {
public:
public:
    JzLight(const glm::vec3 &position, const glm::vec3 &color);
    virtual ~JzLight() = default;

    void             SetPosition(const glm::vec3 &position);
    const glm::vec3 &GetPosition() const;

    void             SetColor(const glm::vec3 &color);
    const glm::vec3 &GetColor() const;

    virtual void ApplyLight(std::shared_ptr<OGLShader> shader, int index) const = 0;

protected:
    glm::vec3 position;
    glm::vec3 color;
};
}; // namespace JzRE