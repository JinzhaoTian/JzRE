#include "SoftwareRenderEngine.h"
#include "GraphicsInterfaceRenderEngine.h"

int main() {
    JzRE::GraphicsInterfaceRenderEngine re;

    if (!re.Initialize()) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return -1;
    }

    re.Run();
    re.Shutdown();

    return 0;
}