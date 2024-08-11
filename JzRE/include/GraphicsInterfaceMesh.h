#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceVertex.h"
#include "GraphicsInterfaceTexture.h"
#include "GraphicsInterfaceShader.h"

namespace JzRE {
class GraphicsInterfaceMesh {
public:
    // mesh Data
    List<GraphicsInterfaceVertex> vertices;
    List<U32> indices;
    List<GraphicsInterfaceTexture> textures;

    GraphicsInterfaceMesh(List<GraphicsInterfaceVertex> vertices, List<U32> indices, List<GraphicsInterfaceTexture> textures);

    void Draw(GraphicsInterfaceShader &shader);

private:
    GLuint VAO, VBO, EBO;

    void SetupMesh();
};

} // namespace JzRE