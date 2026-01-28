/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <vector>
#include "JzRE/Runtime/Resource/JzResource.h"
#include "JzRE/Runtime/Platform/RHI/JzRHIPipeline.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"
#include "JzRE/Runtime/Core/JzVector.h"

namespace JzRE {

/**
 * @brief Material properties structure for MTL file data
 */
struct JzMaterialProperties {
    String name;                                     ///< Material name
    JzVec3 ambientColor  = JzVec3(0.1f, 0.1f, 0.1f); ///< Ka - Ambient color
    JzVec3 diffuseColor  = JzVec3(0.8f, 0.8f, 0.8f); ///< Kd - Diffuse color
    JzVec3 specularColor = JzVec3(0.5f, 0.5f, 0.5f); ///< Ks - Specular color
    F32    shininess     = 32.0f;                    ///< Ns - Shininess
    F32    opacity       = 1.0f;                     ///< d - Opacity (1.0 = opaque)
    String diffuseTexturePath;                       ///< map_Kd - Diffuse texture path
};

/**
 * @brief Represents a material asset.
 *        It holds references to the shader pipeline and textures.
 */
class JzMaterial : public JzResource {
public:
    /**
     * @brief Constructor.
     *
     * @param path File path to the material.
     */
    JzMaterial(const String &path);

    /**
     * @brief Constructor with material properties.
     *
     * @param properties Material properties from MTL file.
     */
    JzMaterial(const JzMaterialProperties &properties);

    /**
     * @brief Destructor
     */
    virtual ~JzMaterial() = default;

    /**
     * @brief Load a resource. For materials, this creates the pipeline.
     *
     * @return Bool True if successful.
     */
    virtual Bool Load() override;

    /**
     * @brief Unload a resource. Releases the pipeline and textures.
     */
    virtual void Unload() override;

    /**
     * @brief Get the RHI Pipeline object
     *
     * @return std::shared_ptr<JzRHIPipeline>
     */
    std::shared_ptr<JzRHIPipeline> GetPipeline() const
    {
        return m_pipeline;
    }

    /**
     * @brief Get the Textures
     *
     * @return const std::vector<std::shared_ptr<JzGPUTextureObject>>&
     */
    const std::vector<std::shared_ptr<JzGPUTextureObject>> &GetTextures() const
    {
        return m_textures;
    }

    /**
     * @brief Get the diffuse texture
     *
     * @return std::shared_ptr<JzGPUTextureObject>
     */
    std::shared_ptr<JzGPUTextureObject> GetDiffuseTexture() const
    {
        return m_diffuseTexture;
    }

    /**
     * @brief Set the diffuse texture
     *
     * @param texture
     */
    void SetDiffuseTexture(std::shared_ptr<JzGPUTextureObject> texture)
    {
        m_diffuseTexture = texture;
    }

    /**
     * @brief Check if material has a diffuse texture
     *
     * @return Bool
     */
    Bool HasDiffuseTexture() const
    {
        return m_diffuseTexture != nullptr;
    }

    /**
     * @brief Get the material properties
     *
     * @return const JzMaterialProperties&
     */
    const JzMaterialProperties &GetProperties() const
    {
        return m_properties;
    }

    /**
     * @brief Set the material properties
     *
     * @param properties
     */
    void SetProperties(const JzMaterialProperties &properties)
    {
        m_properties = properties;
    }

protected:
    JzMaterialProperties                             m_properties;
    std::shared_ptr<JzRHIPipeline>                   m_pipeline;
    std::vector<std::shared_ptr<JzGPUTextureObject>> m_textures;
    std::shared_ptr<JzGPUTextureObject>              m_diffuseTexture; ///< Diffuse texture (map_Kd)
};

} // namespace JzRE
