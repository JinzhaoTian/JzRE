#include "SoftwareRenderEngine.h"

int main() {
    JzRE::UniquePtr<JzRE::SoftwareRenderEngine> renderEngine = JzRE::CreateUniquePtr<JzRE::SoftwareRenderEngine>();
    renderEngine->Render();

    return 0;
}