#pragma once

#include "CommonTypes.h"
#include "OGLVertex.h"
#include "OGLTexture.h"
#include "OGLShader.h"

namespace JzRE {
class OGLMesh {
public:
    // mesh Data
    List<OGLVertex> vertices;
    List<U32> indices;
    List<SharedPtr<OGLTexture>> textures;

    OGLMesh(List<OGLVertex> vertices, List<U32> indices, List<SharedPtr<OGLTexture>> textures);

    void Draw(SharedPtr<OGLShader> shader);

private:
    GLuint VAO, VBO, EBO;

    void SetupMesh();
};

} // namespace JzRE