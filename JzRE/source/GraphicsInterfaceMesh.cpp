#include "GraphicsInterfaceMesh.h"

namespace JzRE {
GraphicsInterfaceMesh::GraphicsInterfaceMesh(List<GraphicsInterfaceVertex> &&vertices, List<U32> &&indices, List<GraphicsInterfaceTexture> &&textures) :
    vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures)) {
    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    SetupMesh();
}

void GraphicsInterfaceMesh::Draw(GraphicsInterfaceShader &shader) {
    // bind appropriate textures
    for (U32 i = 0; i < this->textures.size(); i++) {
        this->textures[i].Bind(i);
        shader.SetUniform(this->textures[i].textureName, StaticCast<I32>(i));
    }

    // draw mesh
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, static_cast<U32>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

// initializes all the buffer objects/arrays
void GraphicsInterfaceMesh::SetupMesh() {
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GraphicsInterfaceVertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(U32), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GraphicsInterfaceVertex), (void *)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GraphicsInterfaceVertex), (void *)offsetof(GraphicsInterfaceVertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GraphicsInterfaceVertex), (void *)offsetof(GraphicsInterfaceVertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GraphicsInterfaceVertex), (void *)offsetof(GraphicsInterfaceVertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(GraphicsInterfaceVertex), (void *)offsetof(GraphicsInterfaceVertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(GraphicsInterfaceVertex), (void *)offsetof(GraphicsInterfaceVertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(GraphicsInterfaceVertex), (void *)offsetof(GraphicsInterfaceVertex, m_Weights));
    glBindVertexArray(0);
}
} // namespace JzRE