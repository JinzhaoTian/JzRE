#pragma once

#include "CommonTypes.h"

namespace JzRE {
class GraphicsInterfaceTexture {
public:
    GraphicsInterfaceTexture();
    GraphicsInterfaceTexture(GraphicsInterfaceTexture&& other) noexcept;
    ~GraphicsInterfaceTexture();

    GraphicsInterfaceTexture& operator=(GraphicsInterfaceTexture&& other) noexcept;

    Bool LoadFromFile(const String &textureName, const String &texturePath);
    void Bind(U32 unit) const;
    void Unbind() const;

    String textureName;
    String texturePath;

private:
    GLuint textureID;

    GraphicsInterfaceTexture(const GraphicsInterfaceTexture&) = delete;
    GraphicsInterfaceTexture& operator=(const GraphicsInterfaceTexture&) = delete;
};
} // namespace JzRE
