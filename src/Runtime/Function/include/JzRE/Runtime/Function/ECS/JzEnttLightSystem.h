/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzEnttSystem.h"
#include "JzRE/Runtime/Function/ECS/JzEnttWorld.h"

namespace JzRE {

/**
 * @brief Light type enumeration.
 */
enum class JzELightType : U32 {
    Directional = 0,
    Point       = 1,
    Spot        = 2
};

/**
 * @brief Collected light data for rendering.
 */
struct JzLightData {
    JzVec3       position;
    JzVec3       direction;
    JzVec3       color;
    F32          intensity;
    F32          range;
    F32          innerCutoff;
    F32          outerCutoff;
    JzELightType type;
};

/**
 * @brief System that collects and prepares light data for the render system.
 *
 * This system gathers all light entities and their properties into a format
 * that can be easily consumed by the render system.
 */
class JzEnttLightSystem : public JzEnttSystem {
public:
    JzEnttLightSystem() = default;

    void OnInit(JzEnttWorld &world) override;
    void Update(JzEnttWorld &world, F32 delta) override;

    /**
     * @brief Light system runs in PreRender phase.
     */
    JzSystemPhase GetPhase() const override
    {
        return JzSystemPhase::PreRender;
    }

    /**
     * @brief Get all collected lights.
     */
    const std::vector<JzLightData> &GetLights() const
    {
        return m_lights;
    }

    /**
     * @brief Get the primary directional light direction (for simple forward shading).
     */
    JzVec3 GetPrimaryLightDirection() const
    {
        return m_primaryLightDir;
    }

    /**
     * @brief Get the primary directional light color.
     */
    JzVec3 GetPrimaryLightColor() const
    {
        return m_primaryLightColor;
    }

    /**
     * @brief Get the primary directional light intensity.
     */
    F32 GetPrimaryLightIntensity() const
    {
        return m_primaryLightIntensity;
    }

private:
    std::vector<JzLightData> m_lights;
    JzVec3                   m_primaryLightDir{0.3f, 1.0f, 0.5f};
    JzVec3                   m_primaryLightColor{1.0f, 1.0f, 1.0f};
    F32                      m_primaryLightIntensity = 1.0f;
};

} // namespace JzRE
