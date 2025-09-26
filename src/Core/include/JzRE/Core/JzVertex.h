/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzVector.h"

namespace JzRE {

/**
 * @brief Represents a vertex in a mesh, containing graphical attributes.
 */
struct JzVertex {
    JzVec3 Position;
    JzVec3 Normal;
    JzVec2 TexCoords;
    JzVec3 Tangent;
    JzVec3 Bitangent;

    // Placeholder for skinning/animation data
    // int BoneIDs[4];
    // float Weights[4];
};

} // namespace JzRE
