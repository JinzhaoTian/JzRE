#include "GraphicsInterfaceRenderer.h"

namespace JzRE {
GraphicsInterfaceRenderer::GraphicsInterfaceRenderer() {
}

GraphicsInterfaceRenderer::~GraphicsInterfaceRenderer() {
    Shutdown();
}

Bool GraphicsInterfaceRenderer::Initialize() {
    // 配置 OpenGL 状态
    glEnable(GL_DEPTH_TEST);

    return true;
}

void GraphicsInterfaceRenderer::Shutdown() {
}

void GraphicsInterfaceRenderer::Render(const GraphicsInterfaceScene &scene) {
}

void GraphicsInterfaceRenderer::RenderObject(const RenderableObject &object) const {
    object.Draw();
}

void GraphicsInterfaceRenderer::Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

} // namespace JzRE