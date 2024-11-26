#pragma once

#include "CommonTypes.h"
#include "Vector.h"
#include "Matrix.h"

namespace JzRE {
class Transform {
public:
    Transform();

    void SetPosition(const glm::vec3 &position);
    void SetRotation(float angle, const glm::vec3 &axis);
    void SetScale(const glm::vec3 &scale);

    glm::mat4 GetModelMatrix() const;

private:
    glm::vec3 position;
    glm::vec3 scale;
    glm::mat4 rotation;
};
} // namespace JzRE