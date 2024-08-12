#include "GraphicsInterfaceResourceManager.h"

namespace JzRE {
GraphicsInterfaceResourceManager &GraphicsInterfaceResourceManager::getInstance() {
    static GraphicsInterfaceResourceManager m_instance;
    return m_instance;
}

SharedPtr<GraphicsInterfaceTexture> GraphicsInterfaceResourceManager::LoadTexture(const String &textureName, const String &texturePath) {
    auto it = textures.find(textureName);
    if (it != textures.end()) {
        return it->second;
    }

    SharedPtr<GraphicsInterfaceTexture> texture = CreateSharedPtr<GraphicsInterfaceTexture>();
    if (!texture->LoadFromFile(textureName, texturePath)) {
        throw std::runtime_error("Failed to load texture: " + textureName);
    }

    textures[textureName] = texture;
    return textures[textureName];
}

SharedPtr<GraphicsInterfaceTexture> GraphicsInterfaceResourceManager::GetTexture(const String &textureName) {
    auto it = textures.find(textureName);
    if (it == textures.end()) {
        throw std::runtime_error("Texture not found: " + textureName);
    }
    return it->second;
}

SharedPtr<GraphicsInterfaceShader> GraphicsInterfaceResourceManager::LoadShader(const String &shaderName, const String &vertexPath, const String &fragmentPath) {
    auto it = shaders.find(shaderName);
    if (it != shaders.end()) {
        return it->second;
    }

    SharedPtr<GraphicsInterfaceShader> shader = CreateSharedPtr<GraphicsInterfaceShader>();
    if (!shader->LoadFromFile(vertexPath, fragmentPath)) {
        throw std::runtime_error("Failed to load shader: " + vertexPath + " and " + fragmentPath);
    }

    shaders[shaderName] = shader;
    return shaders[shaderName];
}

SharedPtr<GraphicsInterfaceShader> GraphicsInterfaceResourceManager::GetShader(const String &shaderName) {
    auto it = shaders.find(shaderName);
    if (it == shaders.end()) {
        throw std::runtime_error("Shader not found: " + shaderName);
    }
    return it->second;
}

void GraphicsInterfaceResourceManager::Clear() {
    textures.clear();
    shaders.clear();
}
} // namespace JzRE