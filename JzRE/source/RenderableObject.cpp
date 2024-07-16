#include "RenderableObject.h"

namespace JzRE {
RenderableObject::RenderableObject() :
    texture(nullptr), shader(nullptr) {
    // 初始化VAO、VBO等OpenGL对象
    GLfloat vertices[] = {
        // 顶点数据（位置、法线、纹理坐标）
        // ...
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

void RenderableObject::SetShader(SharedPtr<GraphicsInterfaceShader> shader) {
    this->shader = shader;
}

SharedPtr<GraphicsInterfaceTexture> RenderableObject::GetTexture() const {
    return this->texture;
}

SharedPtr<GraphicsInterfaceShader> RenderableObject::GetShader() const {
    return this->shader;
}

Transform RenderableObject::GetTransform() const {
    return this->transform;
}

GLuint RenderableObject::GetVAO() const {
    return this->VAO;
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
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
}
} // namespace JzRE