/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRenderSystem.h"
#include "JzEntityManager.h"
#include "JzComponent.h"
#include "JzMesh.h"
#include "JzMaterial.h"

namespace JzRE {

JzRenderSystem::JzRenderSystem(std::shared_ptr<JzRHIDevice> device) :
    m_device(device) { }

void JzRenderSystem::Update(JzEntityManager &manager, F32 delta)
{
    if (!m_device) {
        return;
    }

    // In a real scenario, you would get camera matrices from an entity with a CameraComponent
    // auto& camera_transform = manager.GetComponent<TransformComponent>(camera_entity);
    // auto& camera_component = manager.GetComponent<CameraComponent>(camera_entity);
    // ... calculate view-projection matrix ...

    // The entity manager needs a way to iterate over entities with specific components.
    // This is a conceptual representation. Your actual implementation might differ.
    for (auto entity : manager.View<TransformComponent, MeshComponent, MaterialComponent>()) {
        // 1. Get components from the entity
        auto &transform = manager.GetComponent<TransformComponent>(entity);
        auto &meshComp  = manager.GetComponent<MeshComponent>(entity);
        auto &matComp   = manager.GetComponent<MaterialComponent>(entity);

        // 2. Get JzResource from components
        auto mesh     = std::static_pointer_cast<JzMesh>(meshComp.mesh);
        auto material = std::static_pointer_cast<JzMaterial>(matComp.material);

        // 3. Validate resources
        if (!mesh || !material || mesh->GetState() != JzEResourceState::Loaded || material->GetState() != JzEResourceState::Loaded) {
            continue; // Skip if resources aren't ready
        }

        // 4. Get JzRHIResource from JzResource
        auto pipeline    = material->GetPipeline();
        auto vertexArray = mesh->GetVertexArray();
        auto textures    = material->GetTextures();

        if (!pipeline || !vertexArray) {
            continue; // Skip if essential RHI resources are missing
        }

        // 5. Use RHI Device to issue rendering commands
        m_device->BindPipeline(pipeline);

        // Bind textures
        for (uint32_t i = 0; i < textures.size(); ++i) {
            m_device->BindTexture(textures[i], i);
        }

        // Update uniforms (e.g., Model-View-Projection matrix)
        // This is highly conceptual and depends on your shader and uniform buffer setup.
        // struct TransformUniforms { JzMat4 model; JzMat4 view; JzMat4 proj; };
        // TransformUniforms uniforms;
        // uniforms.model = calculate_model_matrix(transform);
        // uniforms.view = ...;
        // uniforms.proj = ...;
        // m_device->UpdateUniformBuffer(..., &uniforms, sizeof(uniforms));

        m_device->BindVertexArray(vertexArray);

        // 6. Issue draw call
        JzDrawIndexedParams drawParams{};
        drawParams.indexCount = mesh->GetIndexCount();
        m_device->DrawIndexed(drawParams);
    }
}

} // namespace JzRE
