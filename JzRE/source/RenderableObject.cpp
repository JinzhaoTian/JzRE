#include "RenderableObject.h"

namespace JzRE {
RenderableObject::RenderableObject() :
    texture(nullptr), shader(nullptr) {
    // 初始化VAO、VBO等OpenGL对象
    GLfloat vertices[] = {
        // positions          // colors           // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };

    GLuint indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

RenderableObject::~RenderableObject() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void RenderableObject::SetTexture(SharedPtr<GraphicsInterfaceTexture> texture) {
    this->texture = texture;
}

SharedPtr<GraphicsInterfaceTexture> RenderableObject::GetTexture() const {
    return this->texture;
}

void RenderableObject::SetShader(SharedPtr<GraphicsInterfaceShader> shader) {
    this->shader = shader;
}

SharedPtr<GraphicsInterfaceShader> RenderableObject::GetShader() const {
    return this->shader;
}

void RenderableObject::Update(F32 deltaTime) {
    // 这里可以更新物体的逻辑，例如位置变化等
}

void RenderableObject::Draw() const {
    if (this->shader) {
        shader->Use();

        // 设置纹理
        if (texture) {
            texture->Bind(0);
            shader->SetUniform("texture1", 0);
        }

        // 设置模型矩阵
        shader->SetUniform("model", this->transform.GetModelMatrix());

        // 渲染物体
        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
} // namespace JzRE