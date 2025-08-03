#pragma once

#include "CommonTypes.h"

namespace JzRE {

class JzVertex {
public:
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    I32       m_BoneIDs[4];
    F32       m_Weights[4];
};

} // namespace JzRE