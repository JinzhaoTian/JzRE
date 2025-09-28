/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/Core/JzVector.h"
#include "JzRE/UI/JzPlugin.h"
#include "JzRE/RHI/JzRHITexture.h"

namespace JzRE {

/**
 * @brief Texture preview plugin
 */
class JzTexturePreview : public JzPlugin {
public:
    /**
     * @brief Constructor
     *
     * @param texture
     */
    JzTexturePreview(std::shared_ptr<JzRHITexture> texture);

    /**
     * @brief Execute the plugin
     *
     * @param context
     */
    void Execute(JzEPluginExecutionContext context) override;

private:
    JzVec2                        m_textureSize;
    std::shared_ptr<JzRHITexture> m_texture;
};

} // namespace JzRE