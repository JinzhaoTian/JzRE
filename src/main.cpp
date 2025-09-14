/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <memory>
#include <optional>
#include <filesystem>
#include "JzRHIETypes.h"
#include "JzREHub.h"
#include "JzRenderEngine.h"

int main(int argc, char **argv)
{
    auto rhiType = JzRE::JzERHIType::OpenGL;

    std::optional<std::filesystem::path> openPath;

    if (argc < 2) {
        auto hub = std::make_unique<JzRE::JzREHub>(rhiType);

        if (auto result = hub->Run()) {
            openPath = result;
        }
    } else {
        openPath = argv[1];
    }

    if (openPath) {
        if (!std::filesystem::exists(openPath.value())) {
            return EXIT_FAILURE;
        }

        auto re = std::make_unique<JzRE::JzRenderEngine>(rhiType, openPath.value());

        if (re) {
            re->Run();
        } else {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}