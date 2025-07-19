#pragma once

#include "CommonTypes.h"
#include "OGLLight.h"

namespace JzRE {
class OGLDirectionalLight : public OGLLight {
public:
    OGLDirectionalLight(const glm::vec3 &direction, const glm::vec3 &color);

    const glm::vec3 &GetDirection() const;
    void SetDirection(const glm::vec3 &direction);

    void ApplyLight(std::shared_ptr<OGLShader> shader, I32 index) const override;

private:
    glm::vec3 direction;
};
} // namespace JzRE