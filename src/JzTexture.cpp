/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzTexture.h"
#include "JzLogger.h"
#include "JzServiceContainer.h"
#include "JzResourceManager.h"

// For now, we assume the existence of a function to create RHI textures.
// In a real scenario, this would come from the RHI device.
JzRE::JzRHITexture *CreateRHITextureFromFile(const JzRE::String &path);

JzRE::JzTexture::JzTexture(const JzRE::String &name)
{
    m_name = name;
}

JzRE::Bool JzRE::JzTexture::Load()
{
    m_state = JzEResourceState::Loading;

    auto  &resourceManager = JzServiceContainer::Get<JzResourceManager>();
    String fullPath        = resourceManager.FindFullPath(m_name);

    if (fullPath.empty()) {
        JzLogger::GetInstance().Error(fmt::format("Texture file not found: {0}", m_name));
        m_state = JzEResourceState::Error;
        return false;
    }

    // In a real implementation, you would load the image data from the file (e.g., using stb_image)
    // and then create the RHI texture from that data.
    // For this example, we'll assume a helper function exists.
    m_rhiTexture.reset(CreateRHITextureFromFile(fullPath));

    if (m_rhiTexture) {
        JzLogger::GetInstance().Info(fmt::format("Texture loaded: {0}", m_name));
        m_state = JzEResourceState::Loaded;
        return true;
    } else {
        JzLogger::GetInstance().Error(fmt::format("Failed to create RHI texture for: {0}", m_name));
        m_state = JzEResourceState::Error;
        return false;
    }
}

void JzRE::JzTexture::Unload()
{
    m_state = JzEResourceState::Unloaded;
    m_rhiTexture.reset();
    JzLogger::GetInstance().Info(fmt::format("Texture unloaded: {0}", m_name));
}

// Dummy implementation for the example
JzRE::JzRHITexture *CreateRHITextureFromFile(const JzRE::String &path)
{
    // This is a placeholder. In a real engine, you would get the RHI device
    // (e.g., from JzServiceContainer) and call a method like device->CreateTexture2D(path);
    // For now, we can't create a real RHI texture, so we return nullptr to avoid breaking compilation.
    return nullptr;
}