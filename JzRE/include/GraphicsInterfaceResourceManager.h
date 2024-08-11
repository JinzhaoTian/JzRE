#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceShader.h"
#include "GraphicsInterfaceTexture.h"

namespace JzRE {
class GraphicsInterfaceResourceManager {
public:
    static GraphicsInterfaceResourceManager &getInstance();

    GraphicsInterfaceTexture LoadTexture(const String &textureName, const String &texturePath);
    GraphicsInterfaceTexture GetTexture(const String &textureName);

    GraphicsInterfaceShader LoadShader(const String &shaderName, const String &vertexPath, const String &fragmentPath);
    GraphicsInterfaceShader GetShader(const String &shaderName);

    // 清理资源
    void Clear();

private:
    GraphicsInterfaceResourceManager() = default;
    ~GraphicsInterfaceResourceManager() = default;

    GraphicsInterfaceResourceManager(const GraphicsInterfaceResourceManager &) = delete;
    GraphicsInterfaceResourceManager &operator=(const GraphicsInterfaceResourceManager &) = delete;

    UnorderedMap<String, GraphicsInterfaceTexture> textures;
    UnorderedMap<String, GraphicsInterfaceShader> shaders;
};
} // namespace JzRE