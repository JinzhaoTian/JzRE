#include "GraphicsInterfaceResourceManager.h"

namespace JzRE {
GraphicsInterfaceResourceManager &GraphicsInterfaceResourceManager::getInstance() {
    static GraphicsInterfaceResourceManager m_instance;
    return m_instance;
}

GraphicsInterfaceTexture GraphicsInterfaceResourceManager::LoadTexture(const String &textureName, const String &texturePath) {
    auto it = textures.find(textureName);
    if (it != textures.end()) {
        return std::move(it->second);
    }

    GraphicsInterfaceTexture texture;
    if (!texture.LoadFromFile(textureName, texturePath)) {
        throw std::runtime_error("Failed to load texture: " + textureName);
    }

    textures[textureName] = std::move(texture);
    return std::move(textures[textureName]);
}

GraphicsInterfaceTexture GraphicsInterfaceResourceManager::GetTexture(const String &textureName) {
    auto it = textures.find(textureName);
    if (it == textures.end()) {
        throw std::runtime_error("Texture not found: " + textureName);
    }
    return std::move(it->second);
}

GraphicsInterfaceShader GraphicsInterfaceResourceManager::LoadShader(const String &shaderName, const String &vertexPath, const String &fragmentPath) {
    auto it = shaders.find(shaderName);
    if (it != shaders.end()) {
        return std::move(it->second);
    }

    GraphicsInterfaceShader shader;
    if (!shader.LoadFromFile(vertexPath, fragmentPath)) {
        throw std::runtime_error("Failed to load shader: " + vertexPath + " and " + fragmentPath);
    }

    shaders[shaderName] = std::move(shader);
    return std::move(shaders[shaderName]);
}

GraphicsInterfaceShader GraphicsInterfaceResourceManager::GetShader(const String &shaderName) {
    auto it = shaders.find(shaderName);
    if (it == shaders.end()) {
        throw std::runtime_error("Shader not found: " + shaderName);
    }
    return std::move(it->second);
}

void GraphicsInterfaceResourceManager::Clear() {
    textures.clear();
    shaders.clear();
}
} // namespace JzRE