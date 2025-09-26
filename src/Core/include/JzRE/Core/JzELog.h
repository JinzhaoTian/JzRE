/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"

namespace JzRE {
/**
 * @brief Enums of log level
 */
enum class JzELogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

/**
 * @brief Log Message
 */
struct JzLogMessage {
    String      message;
    String      sourceFile;
    String      line;
    String      functionName;
    Size        threadId;
    JzELogLevel level;
};

} // namespace JzRE