/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <utility>

#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief CLI process exit codes.
 */
enum class JzCliExitCode : I32 {
    Success          = 0,
    InvalidArguments = 2,
    IoError          = 3,
    ProjectError     = 4,
    ToolError        = 5,
    RuntimeError     = 6,
};

/**
 * @brief Global CLI output format.
 */
enum class JzCliOutputFormat : U8 {
    Text,
    Json,
};

/**
 * @brief Command execution result.
 */
struct JzCliResult {
    JzCliExitCode code{JzCliExitCode::Success};
    String        message;

    /**
     * @brief Check whether result is successful.
     *
     * @return Bool True when code is Success.
     */
    [[nodiscard]] Bool IsSuccess() const
    {
        return code == JzCliExitCode::Success;
    }

    /**
     * @brief Construct a success result.
     *
     * @param message Message payload.
     *
     * @return JzCliResult Success result.
     */
    static JzCliResult Ok(String message = {})
    {
        return {JzCliExitCode::Success, std::move(message)};
    }

    /**
     * @brief Construct an error result.
     *
     * @param code Non-success exit code.
     * @param message Message payload.
     *
     * @return JzCliResult Error result.
     */
    static JzCliResult Error(JzCliExitCode code, String message)
    {
        return {code, std::move(message)};
    }
};

} // namespace JzRE
