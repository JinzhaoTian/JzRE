/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzResource.h"
#include "JzRHIPipeline.h"
#include "JzRHITexture.h"
#include <memory>
#include <vector>

namespace JzRE {

/**
 * @brief Represents a material asset.
 *        It holds references to the shader pipeline and textures.
 */
class JzMaterial : public JzResource {
public:
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
    std::shared_ptr<JzRHIPipeline> GetPipeline() const { return m_pipeline; }

    /**
     * @brief Get the Textures
     *
     * @return const std::vector<std::shared_ptr<JzRHITexture>>&
     */
    const std::vector<std::shared_ptr<JzRHITexture>>& GetTextures() const { return m_textures; }

protected:
    // RHI resources
    std::shared_ptr<JzRHIPipeline>             m_pipeline;
    std::vector<std::shared_ptr<JzRHITexture>> m_textures;
};

} // namespace JzRE
