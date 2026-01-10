/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/EnttECS/JzEnttSystem.h"
#include "JzRE/EnttECS/JzEnttWorld.h"
#include "JzRE/EnttECS/JzEnttComponents.h"
#include "JzRE/RHI/JzDevice.h"
#include "JzRE/Resource/JzMesh.h"
#include "JzRE/Resource/JzMaterial.h"

namespace JzRE {

/**
 * @brief Render system that draws all renderable entities.
 *
 * This is an EnTT-based reimplementation of the original JzRenderSystem.
 * It demonstrates the EnTT idioms for querying and rendering entities
 * with mesh and material components.
 */
class JzEnttRenderSystem : public JzEnttSystem {
public:
    /**
     * @brief Constructs the render system with a device.
     *
     * @param device The RHI device to use for rendering.
     */
    explicit JzEnttRenderSystem(std::shared_ptr<JzDevice> device) :
        m_device(std::move(device)) { }

    /**
     * @brief Renders all entities with transform, mesh, and material components.
     *
     * @param world The EnTT world containing entities and components.
     * @param delta The delta time since the last frame.
     */
    void Update(JzEnttWorld &world, F32 delta) override
    {
        if (!m_device) {
            return;
        }

        // Create a view for renderable entities
        auto view = world.View<JzTransformComponent, JzMeshComponent, JzMaterialComponent>();

        for (auto [entity, transform, meshComp, matComp] : view.each()) {
            // Get resources from components
            auto mesh     = std::static_pointer_cast<JzMesh>(meshComp.mesh);
            auto material = std::static_pointer_cast<JzMaterial>(matComp.material);

            // Validate resources
            if (!mesh || !material || mesh->GetState() != JzEResourceState::Loaded || material->GetState() != JzEResourceState::Loaded) {
                continue; // Skip if resources aren't ready
            }

            // Get RHI resources
            auto pipeline    = material->GetPipeline();
            auto vertexArray = mesh->GetVertexArray();
            auto textures    = material->GetTextures();

            if (!pipeline || !vertexArray) {
                continue; // Skip if essential RHI resources are missing
            }

            // Issue rendering commands
            m_device->BindPipeline(pipeline);

            // Bind textures
            for (Size i = 0; i < textures.size(); ++i) {
                m_device->BindTexture(textures[i], i);
            }

            m_device->BindVertexArray(vertexArray);

            // Issue draw call
            JzDrawIndexedParams drawParams{};
            drawParams.indexCount = mesh->GetIndexCount();
            m_device->DrawIndexed(drawParams);
        }
    }

private:
    std::shared_ptr<JzDevice> m_device; ///< The RHI device for rendering.
};

} // namespace JzRE
