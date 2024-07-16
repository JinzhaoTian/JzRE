#pragma once

#include "CommonTypes.h"
#include "Object.h"
#include "GraphicsInterfaceShader.h"
#include "GraphicsInterfaceTexture.h"
#include "GraphicsInterfaceRenderer.h"
#include "Transform.h"

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

private:
    GLuint VAO, VBO;

    SharedPtr<GraphicsInterfaceTexture> texture;
    SharedPtr<GraphicsInterfaceShader> shader;
};
} // namespace JzRE