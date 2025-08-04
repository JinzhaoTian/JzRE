#pragma once

#include "CommonTypes.h"
#include "JzVertex.h"
#include "OGLShader.h"
#include "OGLTexture.h"

namespace JzRE {
/**
 * @brief Mesh class
 */
class JzMesh {
public:
    /**
     * @brief Constructor
     */
    JzMesh(std::vector<JzVertex> vertices, std::vector<U32> indices, std::vector<std::shared_ptr<OGLTexture>> textures);

    /**
     * @brief Draw the mesh
     * @param shader The shader to use
     */
    void Draw(std::shared_ptr<OGLShader> shader);

private:
    void SetupMesh();

public:
    std::vector<JzVertex>                    vertices;
    std::vector<U32>                         indices;
    std::vector<std::shared_ptr<OGLTexture>> textures;

private:
    GLuint VAO, VBO, EBO;
};

} // namespace JzRE