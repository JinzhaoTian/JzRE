#include "OGLResourceManager.h"

namespace JzRE {
OGLResourceManager &OGLResourceManager::getInstance() {
    static OGLResourceManager m_instance;
    return m_instance;
}

std::shared_ptr<OGLTexture> OGLResourceManager::LoadTexture(const String &textureName, const String &texturePath) {
    auto it = textures.find(textureName);
    if (it != textures.end()) {
        return it->second;
    }

    std::shared_ptr<OGLTexture> texture = std::make_shared<OGLTexture>();
    if (!texture->LoadFromFile(textureName, texturePath)) {
        throw std::runtime_error("Failed to load texture: " + textureName);
    }

    textures[textureName] = texture;
    return textures[textureName];
}

std::shared_ptr<OGLTexture> OGLResourceManager::GetTexture(const String &textureName) {
    auto it = textures.find(textureName);
    if (it == textures.end()) {
        throw std::runtime_error("Texture not found: " + textureName);
    }
    return it->second;
}

std::shared_ptr<OGLShader> OGLResourceManager::LoadShader(const String &shaderName, const String &vertexPath, const String &fragmentPath) {
    auto it = shaders.find(shaderName);
    if (it != shaders.end()) {
        return it->second;
    }

    std::shared_ptr<OGLShader> shader = std::make_shared<OGLShader>();
    if (!shader->LoadFromFile(vertexPath, fragmentPath)) {
        throw std::runtime_error("Failed to load shader: " + vertexPath + " and " + fragmentPath);
    }

    shaders[shaderName] = shader;
    return shaders[shaderName];
}

std::shared_ptr<OGLShader> OGLResourceManager::GetShader(const String &shaderName) {
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