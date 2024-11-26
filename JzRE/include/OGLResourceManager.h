#pragma once

#include "CommonTypes.h"
#include "OGLShader.h"
#include "OGLTexture.h"

namespace JzRE {
class OGLResourceManager {
public:
    static OGLResourceManager &getInstance();

    SharedPtr<OGLTexture> LoadTexture(const String &textureName, const String &texturePath);
    SharedPtr<OGLTexture> GetTexture(const String &textureName);

    SharedPtr<OGLShader> LoadShader(const String &shaderName, const String &vertexPath, const String &fragmentPath);
    SharedPtr<OGLShader> GetShader(const String &shaderName);

    // 清理资源
    void Clear();

private:
    OGLResourceManager() = default;
    ~OGLResourceManager() = default;

    OGLResourceManager(const OGLResourceManager &) = delete;
    OGLResourceManager &operator=(const OGLResourceManager &) = delete;

    UnorderedMap<String, SharedPtr<OGLTexture>> textures;
    UnorderedMap<String, SharedPtr<OGLShader>> shaders;
};
} // namespace JzRE