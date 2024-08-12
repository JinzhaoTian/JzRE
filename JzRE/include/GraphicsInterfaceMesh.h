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
    List<SharedPtr<GraphicsInterfaceTexture>> textures;

    GraphicsInterfaceMesh(List<GraphicsInterfaceVertex> vertices, List<U32> indices, List<SharedPtr<GraphicsInterfaceTexture>> textures);

    void Draw(SharedPtr<GraphicsInterfaceShader> shader);

private:
    GLuint VAO, VBO, EBO;

    void SetupMesh();
};

} // namespace JzRE