#include "OGLLight.h"

namespace JzRE {
OGLLight::OGLLight(const glm::vec3 &position, const glm::vec3 &color) :
    position(position), color(color) {
}

void OGLLight::SetPosition(const glm::vec3 &position) {
    this->position = position;
}

const glm::vec3 &OGLLight::GetPosition() const {
    return this->position;
}

void OGLLight::SetColor(const glm::vec3 &color) {
    this->color = color;
}

const glm::vec3 &OGLLight::GetColor() const {
    return this->color;
}

} // namespace JzRE