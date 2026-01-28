/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Core/JzMatrix.h"

namespace JzRE {

// ==================== Transform Component ====================

/**
 * @brief Component for position, rotation, and scale with cached world matrix.
 *
 * This component stores transform data directly in a cache-friendly layout.
 * The world matrix is cached and updated lazily when dirty flag is set.
 */
struct JzTransformComponent {
    // Local transform data
    JzVec3 position{0.0f, 0.0f, 0.0f};
    JzVec3 rotation{0.0f, 0.0f, 0.0f}; // Euler angles in radians
    JzVec3 scale{1.0f, 1.0f, 1.0f};

    // Cached matrices
    JzMat4 localMatrix{JzMat4::Identity()};
    JzMat4 worldMatrix{JzMat4::Identity()};

    // Dirty flag for lazy matrix update
    Bool isDirty{true};

    JzTransformComponent() = default;

    JzTransformComponent(const JzVec3 &pos) :
        position(pos), isDirty(true) { }

    JzTransformComponent(const JzVec3 &pos, const JzVec3 &rot, const JzVec3 &scl) :
        position(pos), rotation(rot), scale(scl), isDirty(true) { }

    /**
     * @brief Mark transform as dirty (needs matrix recalculation)
     */
    void SetDirty()
    {
        isDirty = true;
    }

    /**
     * @brief Update local matrix from position, rotation, scale
     */
    void UpdateLocalMatrix()
    {
        if (!isDirty) return;

        // Compute TRS matrix: Translation * RotationZ * RotationY * RotationX * Scale
        JzMat4 T = JzMat4::Translate(position);
        JzMat4 R = JzMat4::RotateZ(rotation.z) * JzMat4::RotateY(rotation.y) * JzMat4::RotateX(rotation.x);
        JzMat4 S = JzMat4::Scale(scale);

        localMatrix = T * R * S;
        worldMatrix = localMatrix; // No parent hierarchy for now
        isDirty     = false;
    }

    /**
     * @brief Get the world matrix, updating if dirty
     */
    const JzMat4 &GetWorldMatrix()
    {
        if (isDirty) {
            UpdateLocalMatrix();
        }
        return worldMatrix;
    }
};

// ==================== Velocity Component ====================

/**
 * @brief Component for velocity.
 */
struct JzVelocityComponent {
    JzVec3 velocity{0.0f, 0.0f, 0.0f};
};

} // namespace JzRE
