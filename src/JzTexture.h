/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzResource.h"
#include "JzRHITexture.h"
#include <memory>

namespace JzRE {

/**
 * @brief Represents a texture asset loaded from an image file.
 */
class JzTexture : public JzResource {
public:
    /**
     * @brief Constructor.
     *
     * @param path The file path to the image.
     */
    JzTexture(const std::string& path);

    /**
     * @brief Destructor.
     */
    virtual ~JzTexture();

    /**
     * @brief Loads the image from file and creates a GPU texture.
     *
     * @return Bool True if successful.
     */
    virtual Bool Load() override;

    /**
     * @brief Unloads the texture, releasing CPU and GPU memory.
     */
    virtual void Unload() override;

    /**
     * @brief Get the RHI Texture object.
     *
     * @return std::shared_ptr<JzRHITexture>
     */
    std::shared_ptr<JzRHITexture> GetRhiTexture() const { return m_rhiTexture; }

private:
    std::string m_path;
    std::shared_ptr<JzRHITexture> m_rhiTexture;
};

} // namespace JzRE