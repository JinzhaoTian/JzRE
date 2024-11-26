#include "OGLResourceManager.h"

namespace JzRE {
OGLResourceManager &OGLResourceManager::getInstance() {
    static OGLResourceManager m_instance;
    return m_instance;
}

SharedPtr<OGLTexture> OGLResourceManager::LoadTexture(const String &textureName, const String &texturePath) {
    auto it = textures.find(textureName);
    if (it != textures.end()) {
        return it->second;
    }

    SharedPtr<OGLTexture> texture = CreateSharedPtr<OGLTexture>();
    if (!texture->LoadFromFile(textureName, texturePath)) {
        throw std::runtime_error("Failed to load texture: " + textureName);
    }

    textures[textureName] = texture;
    return textures[textureName];
}

SharedPtr<OGLTexture> OGLResourceManager::GetTexture(const String &textureName) {
    auto it = textures.find(textureName);
    if (it == textures.end()) {
        throw std::runtime_error("Texture not found: " + textureName);
    }
    return it->second;
}

SharedPtr<OGLShader> OGLResourceManager::LoadShader(const String &shaderName, const String &vertexPath, const String &fragmentPath) {
    auto it = shaders.find(shaderName);
    if (it != shaders.end()) {
        return it->second;
    }

    SharedPtr<OGLShader> shader = CreateSharedPtr<OGLShader>();
    if (!shader->LoadFromFile(vertexPath, fragmentPath)) {
        throw std::runtime_error("Failed to load shader: " + vertexPath + " and " + fragmentPath);
    }

    shaders[shaderName] = shader;
    return shaders[shaderName];
}

SharedPtr<OGLShader> OGLResourceManager::GetShader(const String &shaderName) {
    auto it = shaders.find(shaderName);
    if (it == shaders.end()) {
        throw std::runtime_error("Shader not found: " + shaderName);
    }
    return it->second;
}

void OGLResourceManager::Clear() {
    textures.clear();
    shaders.clear();
}
} // namespace JzRE