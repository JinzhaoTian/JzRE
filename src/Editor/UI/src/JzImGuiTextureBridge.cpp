/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Editor/UI/JzImGuiTextureBridge.h"

#include <cstdint>
#include <mutex>
#include <type_traits>
#include <unordered_map>

#include <imgui_impl_vulkan.h>

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanTexture.h"

namespace JzRE {

namespace {

struct JzImGuiTextureEntry {
    ImTextureID imguiId{};
    JzERHIType   rhiType = JzERHIType::Unknown;
};

std::mutex                                      __BRIDGE_MUTEX;
std::unordered_map<const void *, JzImGuiTextureEntry> __TEXTURE_CACHE;
Bool                                            __INITIALIZED = false;

JzERHIType GetCurrentRHIType()
{
    if (!JzServiceContainer::Has<JzDevice>()) {
        return JzERHIType::Unknown;
    }

    return JzServiceContainer::Get<JzDevice>().GetRHIType();
}

template <typename T = ImTextureID>
requires std::is_pointer_v<T>
ImTextureID ResolveOpenGLTextureId(void *token)
{
    return reinterpret_cast<ImTextureID>(token);
}

template <typename T = ImTextureID>
requires(!std::is_pointer_v<T>)
ImTextureID ResolveOpenGLTextureId(void *token)
{
    return static_cast<ImTextureID>(reinterpret_cast<uintptr_t>(token));
}

template <typename T = ImTextureID>
requires std::is_pointer_v<T>
ImTextureID ResolveVulkanDescriptorSet(VkDescriptorSet descriptorSet)
{
    return reinterpret_cast<ImTextureID>(descriptorSet);
}

template <typename T = ImTextureID>
requires(!std::is_pointer_v<T>)
ImTextureID ResolveVulkanDescriptorSet(VkDescriptorSet descriptorSet)
{
    return static_cast<ImTextureID>(reinterpret_cast<uintptr_t>(descriptorSet));
}

template <typename T = ImTextureID>
requires std::is_pointer_v<T>
VkDescriptorSet ResolveDescriptorSet(ImTextureID textureId)
{
    return reinterpret_cast<VkDescriptorSet>(textureId);
}

template <typename T = ImTextureID>
requires(!std::is_pointer_v<T>)
VkDescriptorSet ResolveDescriptorSet(ImTextureID textureId)
{
    return reinterpret_cast<VkDescriptorSet>(static_cast<uintptr_t>(textureId));
}

Bool IsValidTextureId(ImTextureID textureId)
{
    return textureId != ImTextureID{};
}

ImTextureID ResolveVulkanTextureId(const JzVulkanTexture *texture)
{
    if (!texture) {
        return ImTextureID{};
    }

    const void *key = static_cast<const void *>(texture);

    auto cached = __TEXTURE_CACHE.find(key);
    if (cached != __TEXTURE_CACHE.end()) {
        return cached->second.imguiId;
    }

    const VkDescriptorSet descriptorSet = ImGui_ImplVulkan_AddTexture(
        texture->GetSampler(),
        texture->GetImageView(),
        texture->GetLayout());
    const ImTextureID textureId = ResolveVulkanDescriptorSet(descriptorSet);

    JzImGuiTextureEntry entry;
    entry.imguiId = textureId;
    entry.rhiType = JzERHIType::Vulkan;
    __TEXTURE_CACHE[key] = entry;

    return textureId;
}

} // namespace

void JzImGuiTextureBridge::Initialize()
{
    std::lock_guard<std::mutex> lock(__BRIDGE_MUTEX);
    __INITIALIZED = true;
}

void JzImGuiTextureBridge::Shutdown()
{
    std::lock_guard<std::mutex> lock(__BRIDGE_MUTEX);

    for (const auto &item : __TEXTURE_CACHE) {
        const auto &entry = item.second;
        if (entry.rhiType == JzERHIType::Vulkan && IsValidTextureId(entry.imguiId)) {
            ImGui_ImplVulkan_RemoveTexture(ResolveDescriptorSet(entry.imguiId));
        }
    }

    __TEXTURE_CACHE.clear();
    __INITIALIZED = false;
}

ImTextureID JzImGuiTextureBridge::Resolve(const std::shared_ptr<JzGPUTextureObject> &texture)
{
    return Resolve(texture.get());
}

ImTextureID JzImGuiTextureBridge::Resolve(JzGPUTextureObject *texture)
{
    if (!texture) {
        return ImTextureID{};
    }

    std::lock_guard<std::mutex> lock(__BRIDGE_MUTEX);
    if (!__INITIALIZED) {
        __INITIALIZED = true;
    }

    const auto rhiType = GetCurrentRHIType();
    if (rhiType == JzERHIType::OpenGL) {
        return ResolveOpenGLTextureId(texture->GetTextureID());
    }

    if (rhiType == JzERHIType::Vulkan) {
        auto *vkTexture = dynamic_cast<JzVulkanTexture *>(texture);
        if (vkTexture) {
            return ResolveVulkanTextureId(vkTexture);
        }
    }

    return ResolveOpenGLTextureId(texture->GetTextureID());
}

ImTextureID JzImGuiTextureBridge::ResolveRaw(void *textureToken)
{
    if (!textureToken) {
        return ImTextureID{};
    }

    std::lock_guard<std::mutex> lock(__BRIDGE_MUTEX);
    if (!__INITIALIZED) {
        __INITIALIZED = true;
    }

    const auto rhiType = GetCurrentRHIType();
    if (rhiType == JzERHIType::Vulkan) {
        const auto *vkTexture = static_cast<const JzVulkanTexture *>(textureToken);
        if (vkTexture) {
            return ResolveVulkanTextureId(vkTexture);
        }
    }

    return ResolveOpenGLTextureId(textureToken);
}

} // namespace JzRE
