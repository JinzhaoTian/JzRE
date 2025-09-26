/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <assimp/scene.h>
#include "JzRE/Resource/JzResource.h"
#include "JzRE/Resource/JzMesh.h"
#include "JzRE/Resource/JzMaterial.h"

namespace JzRE {

/**
 * @brief A composite resource representing a model file (e.g., .gltf, .fbx).
 *        It contains the node hierarchy and references to all meshes and materials
 *        loaded from the file.
 */
class JzModel : public JzResource {
public:
    /**
     * @brief Node structure mirroring the model file's scene graph.
     */
    struct Node {
        String           name;
        JzMat4           transform;
        std::vector<U32> meshIndices;
        std::vector<U32> childrenIndices;
    };

    /**
     * @brief Constructor.
     *
     * @param path The file path to the model.
     */
    JzModel(const String &path);

    /**
     * @brief Destructor.
     */
    virtual ~JzModel();

    /**
     * @brief Loads the model file and all its sub-resources.
     *
     * @return Bool True if successful.
     */
    virtual Bool Load() override;

    /**
     * @brief Unloads the model and its sub-resources.
     */
    virtual void Unload() override;

    /**
     * @brief Get the Nodes object
     *
     * @return const std::vector<Node>&
     */
    const std::vector<Node> &GetNodes() const
    {
        return m_nodes;
    }

    /**
     * @brief Get the Meshes object
     *
     * @return const std::vector<std::shared_ptr<JzMesh>>&
     */
    const std::vector<std::shared_ptr<JzMesh>> &GetMeshes() const
    {
        return m_meshes;
    }

    /**
     * @brief Get the Materials object
     *
     * @return const std::vector<std::shared_ptr<JzMaterial>>&
     */
    const std::vector<std::shared_ptr<JzMaterial>> &GetMaterials() const
    {
        return m_materials;
    }

private:
    void                        ProcessNode(aiNode *node, const aiScene *scene);
    std::shared_ptr<JzMesh>     ProcessMesh(aiMesh *mesh, const aiScene *scene);
    std::shared_ptr<JzMaterial> ProcessMaterial(aiMaterial *mat, const aiScene *scene);
    // Placeholder for texture loading logic
    // std::shared_ptr<JzTexture> LoadTexture(const std::string& path);

private:
    String                                   m_path;
    String                                   m_directory;
    std::vector<Node>                        m_nodes;
    std::vector<std::shared_ptr<JzMesh>>     m_meshes;
    std::vector<std::shared_ptr<JzMaterial>> m_materials;
};

} // namespace JzRE
