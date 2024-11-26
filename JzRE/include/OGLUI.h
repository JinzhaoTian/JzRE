#pragma once

#include "CommonTypes.h"

namespace JzRE {
class OGLUI {
public:
    OGLUI(RawPtr<GLFWwindow> window);
    ~OGLUI();

    void Render();

    void SetCallbackOpenFile(Callback<String> callback);

private:
    I32 dialogWidth = 600;
    I32 dialogHeight = 450;

    Callback<String> callbackOpenFile;
};
} // namespace JzRE