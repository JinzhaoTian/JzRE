/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzTexture.h"
#include "JzContext.h"
#include "JzRHIDevice.h"
#include "JzRHIDescription.h"

// vcpkg provides stb_image, so we can include it directly.
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace JzRE {

JzTexture::JzTexture(const std::string& path) : m_path(path) {
    m_state = JzEResourceState::Unloaded;
}

JzTexture::~JzTexture() {
    Unload();
}

Bool JzTexture::Load() {
    if (m_state == JzEResourceState::Loaded) {
        return true;
    }
    m_state = JzEResourceState::Loading;

    int width, height, channels;
    // Force 4 channels (RGBA) for consistency
    stbi_uc* pixels = stbi_load(m_path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!pixels) {
        m_state = JzEResourceState::Error;
        // In a real engine, log an error here.
        return false;
    }

    auto& device = JzRE_DEVICE();

    JzTextureDesc textureDesc;
    textureDesc.width     = width;
    textureDesc.height    = height;
    textureDesc.format    = JzETextureFormat::RGBA8;
    textureDesc.debugName = m_path;
    textureDesc.data      = pixels;

    m_rhiTexture = device.CreateTexture(textureDesc);

    stbi_image_free(pixels);

    if (m_rhiTexture) {
        m_state = JzEResourceState::Loaded;
        return true;
    } else {
        m_state = JzEResourceState::Error;
        return false;
    }
}

void JzTexture::Unload() {
    m_rhiTexture = nullptr;
    m_state      = JzEResourceState::Unloaded;
}

} // namespace JzRE
