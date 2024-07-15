#include "GraphicsInterfaceRenderer.h"

namespace JzRE {
GraphicsInterfaceRenderer::GraphicsInterfaceRenderer() {
}

GraphicsInterfaceRenderer::~GraphicsInterfaceRenderer() {
    Shutdown();
}

bool GraphicsInterfaceRenderer::Initialize() {
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    // 配置 OpenGL 状态
    glEnable(GL_DEPTH_TEST);
    
    return true;
}

void GraphicsInterfaceRenderer::Shutdown() {
    shader.Shutdown();
}

void GraphicsInterfaceRenderer::Render(const GraphicsInterfaceScene &scene) {
}

void GraphicsInterfaceRenderer::Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

} // namespace JzRE