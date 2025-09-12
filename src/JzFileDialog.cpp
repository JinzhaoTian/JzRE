/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzFileDialog.h"

// Platform-specific implementations
#ifdef _WIN32
#include "platform/Windows/JzFileDialogWindows.h"
#elif defined(__APPLE__)
#include "platform/macOS/JzFileDialogMacOS.h"
#elif defined(__linux__)
#include "platform/Linux/JzFileDialogLinux.h"
#endif

std::unique_ptr<JzRE::JzFileDialog> JzRE::JzFileDialog::Create(const JzRE::String &p_dialogTitle)
{
#ifdef _WIN32
    return std::make_unique<JzRE::JzFileDialogWindows>(p_dialogTitle);
#elif defined(__APPLE__)
    return std::make_unique<JzRE::JzFileDialogMacOS>(p_dialogTitle);
#elif defined(__linux__)
    return std::make_unique<JzRE::JzFileDialogLinux>(p_dialogTitle);
#else
    // Fallback or unsupported platform
    return nullptr;
#endif
}
