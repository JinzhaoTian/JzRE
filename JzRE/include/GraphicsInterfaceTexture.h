#pragma once

#include "CommonTypes.h"

namespace JzRE {
class GraphicsInterfaceTexture {
public:
    GraphicsInterfaceTexture();
    ~GraphicsInterfaceTexture();

    Bool LoadFromFile(const String &textureName, const String &texturePath);
    void Bind(U32 unit) const;
    void Unbind() const;

    String textureName;
    String texturePath;

private:
    GLuint textureID;
};
} // namespace JzRE
