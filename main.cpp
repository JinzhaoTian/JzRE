#include "SoftwareRenderEngine.h"
#include "OGLRenderEngine.h"

int main() {
    JzRE::OGLRenderEngine re;

    if (!re.Initialize()) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return -1;
    }

    re.Run();
    re.Shutdown();

    return 0;
}