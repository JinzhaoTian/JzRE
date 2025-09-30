/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Resource/JzTexture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "JzRE/Core/JzServiceContainer.h"
#include "JzRE/RHI/JzDevice.h"

JzRE::JzTexture::JzTexture(std::shared_ptr<JzRE::JzRHITexture> rhiTexture) :
    m_rhiTexture(rhiTexture)
{
    m_state = JzEResourceState::Loaded;
}

JzRE::JzTexture::JzTexture(const std::string &path) :
    m_path(path)
{
    m_state = JzEResourceState::Unloaded;
}

JzRE::JzTexture::~JzTexture()
{
    Unload();
}

JzRE::Bool JzRE::JzTexture::Load()
{
    if (m_state == JzEResourceState::Loaded) {
        return true;
    }
    m_state = JzEResourceState::Loading;

    I32 width, height, channels;
    // Force 4 channels (RGBA) for consistency
    stbi_uc *pixels = stbi_load(m_path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!pixels) {
        m_state = JzEResourceState::Error;
        // In a real engine, log an error here.
        return false;
    }

    auto &device = JzServiceContainer::Get<JzDevice>();

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

void JzRE::JzTexture::Unload()
{
    m_rhiTexture = nullptr;
    m_state      = JzEResourceState::Unloaded;
}
