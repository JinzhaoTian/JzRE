#pragma once

#include "CommonTypes.h"

namespace JzRE {
class OGLUI {
public:
    OGLUI(GLFWwindow* window);
    ~OGLUI();

    void Render();

    void SetCallbackOpenFile(std::function<void(String)> callback);

private:
    I32 dialogWidth = 600;
    I32 dialogHeight = 450;

    std::function<void(String)> callbackOpenFile;
};
} // namespace JzRE