#define STB_IMAGE_IMPLEMENTATION
#include "OGLTexture.h"

namespace JzRE {
OGLTexture::OGLTexture() {
    glGenTextures(1, &textureID);
}

OGLTexture::OGLTexture(OGLTexture &&other) noexcept :
    textureID(other.textureID), textureName(std::move(other.textureName)), texturePath(std::move(other.texturePath)) {
    other.textureID = 0;
}

OGLTexture::~OGLTexture() {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
    }
}

OGLTexture &OGLTexture::operator=(OGLTexture &&other) noexcept {
    if (this != &other) {
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
        }

        textureID = other.textureID;
        textureName = std::move(other.textureName);
        texturePath = std::move(other.texturePath);

        other.textureID = 0;
    }
    return *this;
}

Bool OGLTexture::LoadFromFile(const String &textureName, const String &texturePath) {
    this->textureName = textureName;
    this->texturePath = texturePath;

    glBindTexture(GL_TEXTURE_2D, textureID);

    I32 width, height, nrChannels;
    unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        return true;
    } else {
        std::cerr << "Failed to load texture: " << texturePath << std::endl;
        stbi_image_free(data);
        return false;
    }
}

void OGLTexture::Bind(U32 unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void OGLTexture::Unbind() const {
    glActiveTexture(GL_TEXTURE0);
}

} // namespace JzRE