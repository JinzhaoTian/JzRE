#include "GraphicsInterfaceResourceManager.h"

namespace JzRE {
GraphicsInterfaceResourceManager &GraphicsInterfaceResourceManager::getInstance() {
    static GraphicsInterfaceResourceManager m_instance;
    return m_instance;
}

SharedPtr<GraphicsInterfaceTexture> GraphicsInterfaceResourceManager::LoadTexture(const String &name, const String &filepath) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    }

    SharedPtr<GraphicsInterfaceTexture> texture = CreateSharedPtr<GraphicsInterfaceTexture>();
    if (texture->LoadFromFile(filepath)) {
        textures[name] = texture;
        return texture;
    } else {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        return nullptr;
    }
}

SharedPtr<GraphicsInterfaceTexture> GraphicsInterfaceResourceManager::GetTexture(const String &name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    } else {
        std::cerr << "Texture not found: " << name << std::endl;
        return nullptr;
    }
}

SharedPtr<GraphicsInterfaceShader> GraphicsInterfaceResourceManager::LoadShader(const String &name, const String &vertexPath, const String &fragmentPath) {
    auto it = shaders.find(name);
    if (it != shaders.end()) {
        return it->second;
    }

    SharedPtr<GraphicsInterfaceShader> shader = CreateSharedPtr<GraphicsInterfaceShader>();
    if (shader->LoadFromFile(vertexPath, fragmentPath)) {
        shaders[name] = shader;
        return shader;
    } else {
        std::cerr << "Failed to load shader: " << vertexPath << " and " << fragmentPath << std::endl;
        return nullptr;
    }
}

SharedPtr<GraphicsInterfaceShader> GraphicsInterfaceResourceManager::GetShader(const String &name) {
    auto it = shaders.find(name);
    if (it != shaders.end()) {
        return it->second;
    } else {
        std::cerr << "Shader not found: " << name << std::endl;
        return nullptr;
    }
}

void GraphicsInterfaceResourceManager::Clear() {
    textures.clear();
    shaders.clear();
}
} // namespace JzRE