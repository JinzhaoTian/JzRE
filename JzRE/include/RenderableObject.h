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

    void Update(F32 deltaTime) override;
    void Draw() const override;

    glm::mat4 GetModelMatrix() const;

private:
    GLuint VAO, VBO, EBO;
};
} // namespace JzRE