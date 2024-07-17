#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceShader.h"
#include "GraphicsInterfaceTexture.h"

namespace JzRE {
class GraphicsInterfaceResourceManager {
public:
    static GraphicsInterfaceResourceManager &getInstance();

    SharedPtr<GraphicsInterfaceTexture> LoadTexture(const String &name, const String &filepath);
    SharedPtr<GraphicsInterfaceTexture> GetTexture(const String &name);

    SharedPtr<GraphicsInterfaceShader> LoadShader(const String &name, const String &vertexPath, const String &fragmentPath);
    SharedPtr<GraphicsInterfaceShader> GetShader(const String &name);

    // 清理资源
    void Clear();

private:
    GraphicsInterfaceResourceManager() = default;
    ~GraphicsInterfaceResourceManager() = default;

    GraphicsInterfaceResourceManager(const GraphicsInterfaceResourceManager &) = delete;
    GraphicsInterfaceResourceManager &operator=(const GraphicsInterfaceResourceManager &) = delete;

    UnorderedMap<String, SharedPtr<GraphicsInterfaceTexture>> textures;
    UnorderedMap<String, SharedPtr<GraphicsInterfaceShader>> shaders;
};
} // namespace JzRE