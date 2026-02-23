/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <memory>

#include <imgui.h>

#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

class JzGPUTextureObject;

/**
 * @brief Bridge layer that maps engine texture objects to ImGui texture IDs.
 *
 * OpenGL path returns GLuint-backed ImTextureID directly.
 * Vulkan path allocates and caches descriptor sets via ImGui_ImplVulkan.
 */
class JzImGuiTextureBridge {
public:
    /**
     * @brief Initialize bridge state.
     */
    static void Initialize();

    /**
     * @brief Shutdown bridge state and release cached resources.
     */
    static void Shutdown();

    /**
     * @brief Resolve ImGui texture id from a texture shared pointer.
     */
    static ImTextureID Resolve(const std::shared_ptr<JzGPUTextureObject> &texture);

    /**
     * @brief Resolve ImGui texture id from a raw texture object pointer.
     */
    static ImTextureID Resolve(JzGPUTextureObject *texture);

    /**
     * @brief Resolve ImGui texture id from existing raw token.
     *
     * @param textureToken Existing texture token from runtime rendering output.
     */
    static ImTextureID ResolveRaw(void *textureToken);

private:
    JzImGuiTextureBridge() = delete;
};

} // namespace JzRE
