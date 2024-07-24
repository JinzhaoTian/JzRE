#pragma once

#include "CommonTypes.h"
#include "Object.h"
#include "GraphicsInterfaceShader.h"
#include "GraphicsInterfaceTexture.h"

namespace JzRE {
class RenderableObject : public Object {
public:
    RenderableObject();
    ~RenderableObject();

    void SetTexture(SharedPtr<GraphicsInterfaceTexture> texture);
    void SetShader(SharedPtr<GraphicsInterfaceShader> shader);

    SharedPtr<GraphicsInterfaceTexture> GetTexture() const;
    SharedPtr<GraphicsInterfaceShader> GetShader() const;

    Transform GetTransform() const;

    GLuint GetVAO() const;

    void Update(F32 deltaTime) override;
    void Draw() const override;

    void SetProjectionMatrix(glm::mat4);
    void SetViewMatrix(glm::mat4);

private:
    GLuint VAO, VBO, EBO;

    SharedPtr<GraphicsInterfaceTexture> texture;
    SharedPtr<GraphicsInterfaceShader> shader;

    glm::mat4 projectionMatrix;
glm::mat4 viewMatrix;
};
} // namespace JzRE