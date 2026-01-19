/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzEnttLightSystem.h"

#include "JzRE/Runtime/Function/ECS/JzEnttComponents.h"

namespace JzRE {

void JzEnttLightSystem::OnInit(JzEnttWorld &world)
{
    // Nothing to initialize
}

void JzEnttLightSystem::Update(JzEnttWorld &world, F32 delta)
{
    // Clear previous frame's light data
    m_lights.clear();

    Bool foundPrimaryDirectional = false;

    // Collect directional lights
    {
        auto view = world.View<JzTransformComponent, JzEnttDirectionalLightComponent>();
        for (auto entity : view) {
            auto &lightComp = world.GetComponent<JzEnttDirectionalLightComponent>(entity);

            JzLightData data;
            data.position    = JzVec3(0.0f, 0.0f, 0.0f); // Directional lights have no position
            data.direction   = lightComp.direction.Normalized();
            data.color       = lightComp.color;
            data.intensity   = lightComp.intensity;
            data.range       = 0.0f;
            data.innerCutoff = 0.0f;
            data.outerCutoff = 0.0f;
            data.type        = JzELightType::Directional;

            m_lights.push_back(data);

            // Use the first directional light as the primary light
            if (!foundPrimaryDirectional) {
                m_primaryLightDir       = -data.direction; // Negate for light direction toward surface
                m_primaryLightColor     = data.color;
                m_primaryLightIntensity = data.intensity;
                foundPrimaryDirectional = true;
            }
        }
    }

    // Collect point lights
    {
        auto view = world.View<JzTransformComponent, JzEnttPointLightComponent>();
        for (auto entity : view) {
            auto &transform = world.GetComponent<JzTransformComponent>(entity);
            auto &lightComp = world.GetComponent<JzEnttPointLightComponent>(entity);

            JzLightData data;
            data.position    = transform.position;
            data.direction   = JzVec3(0.0f, 0.0f, 0.0f); // Point lights have no direction
            data.color       = lightComp.color;
            data.intensity   = lightComp.intensity;
            data.range       = lightComp.range;
            data.innerCutoff = 0.0f;
            data.outerCutoff = 0.0f;
            data.type        = JzELightType::Point;

            m_lights.push_back(data);
        }
    }

    // Collect spot lights
    {
        auto view = world.View<JzTransformComponent, JzEnttSpotLightComponent>();
        for (auto entity : view) {
            auto &transform = world.GetComponent<JzTransformComponent>(entity);
            auto &lightComp = world.GetComponent<JzEnttSpotLightComponent>(entity);

            JzLightData data;
            data.position    = transform.position;
            data.direction   = lightComp.direction.Normalized();
            data.color       = lightComp.color;
            data.intensity   = lightComp.intensity;
            data.range       = lightComp.range;
            data.innerCutoff = lightComp.innerCutoff;
            data.outerCutoff = lightComp.outerCutoff;
            data.type        = JzELightType::Spot;

            m_lights.push_back(data);
        }
    }

    // If no directional light found, use default
    if (!foundPrimaryDirectional) {
        m_primaryLightDir       = JzVec3(0.3f, 1.0f, 0.5f).Normalized();
        m_primaryLightColor     = JzVec3(1.0f, 1.0f, 1.0f);
        m_primaryLightIntensity = 1.0f;
    }
}

} // namespace JzRE
