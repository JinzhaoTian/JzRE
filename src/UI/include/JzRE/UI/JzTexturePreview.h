/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/Core/JzVector.h"
#include "JzRE/UI/JzPlugin.h"
#include "JzRE/RHI/JzGPUTextureObject.h"

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
    JzTexturePreview(std::shared_ptr<JzGPUTextureObject> texture);

    /**
     * @brief Execute the plugin
     *
     * @param context
     */
    void Execute(JzEPluginExecutionContext context) override;

private:
    JzVec2                              m_textureSize;
    std::shared_ptr<JzGPUTextureObject> m_texture;
};

} // namespace JzRE