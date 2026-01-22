/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzModelSpawner.h"

#include "JzRE/Runtime/Function/ECS/JzComponents.h"

namespace JzRE {

std::vector<JzEntity> JzModelSpawner::SpawnModel(JzWorld                &world,
                                                         std::shared_ptr<JzModel>    model,
                                                         const JzTransformComponent &rootTransform)
{
    std::vector<JzEntity> entities;

    if (!model) {
        return entities;
    }

    const auto &meshes    = model->GetMeshes();
    const auto &materials = model->GetMaterials();

    for (Size i = 0; i < meshes.size(); ++i) {
        auto &mesh = meshes[i];
        if (!mesh) {
            continue;
        }

        // Get material for this mesh
        std::shared_ptr<JzMaterial> material;
        I32                         matIndex = mesh->GetMaterialIndex();
        if (matIndex >= 0 && static_cast<Size>(matIndex) < materials.size()) {
            material = materials[matIndex];
        }

        // Create entity
        JzEntity entity = SpawnMesh(world, mesh, material, rootTransform);
        entities.push_back(entity);
    }

    return entities;
}

JzEntity JzModelSpawner::SpawnMesh(JzWorld                &world,
                                           std::shared_ptr<JzMesh>     mesh,
                                           std::shared_ptr<JzMaterial> material,
                                           const JzTransformComponent &transform,
                                           const String               &name)
{
    JzEntity entity = world.CreateEntity();

    // Add transform component
    auto &transformComp = world.AddComponent<JzTransformComponent>(entity);
    transformComp       = transform;

    // Add mesh component - copy GPU resources from JzMesh
    auto &meshComp = world.AddComponent<JzMeshComponent>(entity);
    if (mesh) {
        meshComp.vertexArray   = mesh->GetVertexArray();
        meshComp.indexCount    = mesh->GetIndexCount();
        meshComp.materialIndex = mesh->GetMaterialIndex();
        meshComp.isGPUReady    = (meshComp.vertexArray != nullptr);
    }

    // Add material component - copy properties from JzMaterial
    auto &matComp = world.AddComponent<JzMaterialComponent>(entity);
    if (material) {
        const auto &props     = material->GetProperties();
        matComp.ambientColor  = props.ambientColor;
        matComp.diffuseColor  = props.diffuseColor;
        matComp.specularColor = props.specularColor;
        matComp.shininess     = props.shininess;
        matComp.opacity       = props.opacity;
        matComp.baseColor     = JzVec4(props.diffuseColor.x, props.diffuseColor.y,
                                       props.diffuseColor.z, props.opacity);
        matComp.transparent   = props.opacity < 1.0f;
    }

    // Add renderable tag (empty struct, use emplace directly)
    world.GetRegistry().emplace<JzRenderableTag>(entity);

    // Add name component if provided
    if (!name.empty()) {
        world.AddComponent<JzNameComponent>(entity, name);
    }

    return entity;
}

void JzModelSpawner::DestroyEntities(JzWorld                     &world,
                                         const std::vector<JzEntity> &entities)
{
    for (auto entity : entities) {
        if (world.IsValid(entity)) {
            world.DestroyEntity(entity);
        }
    }
}

} // namespace JzRE
