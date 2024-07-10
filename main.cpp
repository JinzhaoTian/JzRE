#include "SoftwareRenderEngine.h"
#include "GraphicsInterfaceRenderEngine.h"

int main() {
    JzRE::UniquePtr<JzRE::GraphicsInterfaceRenderEngine> renderEngine = JzRE::CreateUniquePtr<JzRE::GraphicsInterfaceRenderEngine>();
    renderEngine->Render();

    return 0;
}