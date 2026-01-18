/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/JzEnttEntity.h"
#include "JzRE/Runtime/Function/ECS/JzEnttWorld.h"
#include "JzRE/Runtime/Function/ECS/JzComponent.h"
#include "JzRE/Runtime/Resource/JzMaterial.h"
#include "JzRE/Runtime/Resource/JzMesh.h"
#include "JzRE/Runtime/Resource/JzModel.h"

namespace JzRE {

/**
 * @brief Utility class to spawn ECS entities from JzModel resources.
 *
 * This class provides static methods to convert model resources into
 * ECS entities with appropriate components for rendering.
 */
class JzEnttModelSpawner {
public:
    /**
     * @brief Spawn entities for all meshes in a model.
     *
     * Creates one entity per mesh, each with:
     * - JzTransformComponent
     * - JzMeshComponent
     * - JzMaterialComponent
     * - JzRenderableTag (optional)
     *
     * @param world The ECS world.
     * @param model The model to spawn.
     * @param rootTransform Transform to apply to all entities.
     * @return Vector of created entities.
     */
    static std::vector<JzEnttEntity> SpawnModel(JzEnttWorld                   &world,
                                                std::shared_ptr<JzModel>       model,
                                                const JzTransformComponent    &rootTransform = {});

    /**
     * @brief Spawn a single mesh as an entity.
     *
     * @param world The ECS world.
     * @param mesh The mesh resource.
     * @param material The material resource (can be nullptr for default material).
     * @param transform The transform for the entity.
     * @param name Optional name for the entity.
     * @return The created entity.
     */
    static JzEnttEntity SpawnMesh(JzEnttWorld                    &world,
                                  std::shared_ptr<JzMesh>         mesh,
                                  std::shared_ptr<JzMaterial>     material,
                                  const JzTransformComponent     &transform = {},
                                  const String                   &name      = "");

    /**
     * @brief Destroy all entities spawned from a model.
     *
     * @param world The ECS world.
     * @param entities The entities to destroy.
     */
    static void DestroyEntities(JzEnttWorld &world, const std::vector<JzEnttEntity> &entities);
};

} // namespace JzRE
