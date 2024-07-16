#include "Transform.h"

namespace JzRE {
Transform::Transform() :
    position(0.0f), scale(1.0f), rotation(1.0f) {
}

void Transform::SetPosition(const glm::vec3 &position) {
    this->position = position;
}

void Transform::SetRotation(float angle, const glm::vec3 &axis) {
    this->rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
}

void Transform::SetScale(const glm::vec3 &scale) {
    this->scale = scale;
}

glm::mat4 Transform::GetModelMatrix() const {
    return glm::translate(glm::mat4(1.0f), position) * rotation * glm::scale(glm::mat4(1.0f), scale);
}
} // namespace JzRE