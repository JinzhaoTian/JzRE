/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/ECS/JzRenderSystem.h"
#include "JzRE/ECS/JzEntityManager.h"
#include "JzRE/ECS/JzComponent.h"
#include "JzRE/Resource/JzMesh.h"
#include "JzRE/Resource/JzMaterial.h"
#include "JzRE/Editor/JzContext.h"
#include "JzRE/RHI/JzRenderCommand.h"

JzRE::JzRenderSystem::JzRenderSystem() { }

void JzRE::JzRenderSystem::Update(JzRE::JzEntityManager &manager, JzRE::F32 delta)
{
    auto& renderFrontend = JzContext::GetInstance().GetRenderFrontend();

    // The entity manager needs a way to iterate over entities with specific components.
    for (auto entity : manager.View<JzTransformComponent, JzMeshComponent, JzMaterialComponent>()) {
        // 1. Get components from the entity
        auto &transform = manager.GetComponent<JzTransformComponent>(entity);
        auto &meshComp  = manager.GetComponent<JzMeshComponent>(entity);
        auto &matComp   = manager.GetComponent<JzMaterialComponent>(entity);

        // 2. Get JzResource from components
        auto mesh     = std::static_pointer_cast<JzMesh>(meshComp.mesh);
        auto material = std::static_pointer_cast<JzMaterial>(matComp.material);

        // 3. Validate resources
        if (!mesh || !material || mesh->GetState() != JzEResourceState::Loaded || material->GetState() != JzEResourceState::Loaded) {
            continue; // Skip if resources aren't ready
        }

        // 4. Get JzRHIResource from JzResource
        auto pipeline    = material->GetPipeline();
        auto vertexBuffer = mesh->GetVertexArray()->GetVertexBuffers()[0]; // Assuming first vertex buffer
        auto indexBuffer = mesh->GetVertexArray()->GetIndexBuffer();

        if (!pipeline || !vertexBuffer || !indexBuffer) {
            continue; // Skip if essential RHI resources are missing
        }

        // 5. Create a render command
        DrawMeshCommand cmd;
        cmd.pipelineState = pipeline;
        cmd.vertexBuffer = vertexBuffer;
        cmd.indexBuffer = indexBuffer;
        cmd.transform = transform.GetTransform(); // Or whatever gets the model matrix

        // 6. Submit the command to the frontend
        renderFrontend.Submit(cmd);
    }
}
