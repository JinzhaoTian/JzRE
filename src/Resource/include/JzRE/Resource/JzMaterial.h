/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <vector>
#include "JzRE/Resource/JzResource.h"
#include "JzRE/RHI/JzRHIPipeline.h"
#include "JzRE/RHI/JzGPUTextureObject.h"

namespace JzRE {

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

protected:
    // GPU-side RHI resources
    std::shared_ptr<JzRHIPipeline>                   m_pipeline;
    std::vector<std::shared_ptr<JzGPUTextureObject>> m_textures;
};

} // namespace JzRE
