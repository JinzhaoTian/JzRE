#pragma once

#include "CommonTypes.h"
#include "OGLShader.h"
#include "OGLTexture.h"

namespace JzRE {
class OGLResourceManager {
public:
    static OGLResourceManager &getInstance();

    std::shared_ptr<OGLTexture> LoadTexture(const String &textureName, const String &texturePath);
    std::shared_ptr<OGLTexture> GetTexture(const String &textureName);

    std::shared_ptr<OGLShader> LoadShader(const String &shaderName, const String &vertexPath, const String &fragmentPath);
    std::shared_ptr<OGLShader> GetShader(const String &shaderName);

    // 清理资源
    void Clear();

private:
    OGLResourceManager() = default;
    ~OGLResourceManager() = default;

    OGLResourceManager(const OGLResourceManager &) = delete;
    OGLResourceManager &operator=(const OGLResourceManager &) = delete;

    std::unordered_map<String, std::shared_ptr<OGLTexture>> textures;
    std::unordered_map<String, std::shared_ptr<OGLShader>> shaders;
};
} // namespace JzRE