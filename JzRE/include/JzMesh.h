#pragma once

#include "CommonTypes.h"
#include "JzVertex.h"
#include "OGLShader.h"
#include "OGLTexture.h"

namespace JzRE {
class JzMesh {
public:
    // mesh Data
    std::vector<JzVertex>                    vertices;
    std::vector<U32>                         indices;
    std::vector<std::shared_ptr<OGLTexture>> textures;

    JzMesh(std::vector<JzVertex> vertices, std::vector<U32> indices, std::vector<std::shared_ptr<OGLTexture>> textures);

    void Draw(std::shared_ptr<OGLShader> shader);

private:
    GLuint VAO, VBO, EBO;

    void SetupMesh();
};

} // namespace JzRE