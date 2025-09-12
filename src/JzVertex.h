/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRETypes.h"
#include "JzVector.h"

namespace JzRE {
/**
 * @brief Vertex class
 */
class JzVertex {
public:
    JzVec3 Position;   // The vertex's Position
    JzVec3 Normal;     // The vertex's Normal
    JzVec2 TexCoords;  // The vertex's Texture coordinates
    JzVec3 Tangent;    // The vertex's Tangent
    JzVec3 Bitangent;  // The vertex's Bitangent
    I32    BoneIDs[4]; // The vertex's Bone IDs
    F32    Weights[4]; // The vertex's Weights
};

} // namespace JzRE