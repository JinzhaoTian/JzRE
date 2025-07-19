#pragma once

#include "CommonTypes.h"
#include "OGLVertex.h"
#include "OGLTexture.h"
#include "OGLShader.h"

namespace JzRE {
class OGLMesh {
public:
    // mesh Data
    std::vector<OGLVertex> vertices;
    std::vector<U32> indices;
    std::vector<std::shared_ptr<OGLTexture>> textures;

    OGLMesh(std::vector<OGLVertex> vertices, std::vector<U32> indices, std::vector<std::shared_ptr<OGLTexture>> textures);

    void Draw(std::shared_ptr<OGLShader> shader);

private:
    GLuint VAO, VBO, EBO;

    void SetupMesh();
};

} // namespace JzRE