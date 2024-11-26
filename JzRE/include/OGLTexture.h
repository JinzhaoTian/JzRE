#pragma once

#include "CommonTypes.h"

namespace JzRE {
class OGLTexture {
public:
    OGLTexture();
    OGLTexture(OGLTexture&& other) noexcept;
    ~OGLTexture();

    OGLTexture& operator=(OGLTexture&& other) noexcept;

    Bool LoadFromFile(const String &textureName, const String &texturePath);
    void Bind(U32 unit) const;
    void Unbind() const;

    String textureName;
    String texturePath;

private:
    GLuint textureID;

    OGLTexture(const OGLTexture&) = delete;
    OGLTexture& operator=(const OGLTexture&) = delete;
};
} // namespace JzRE
