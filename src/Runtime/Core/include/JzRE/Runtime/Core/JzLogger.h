/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <functional>
#include <spdlog/logger.h>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzELog.h"

namespace JzRE {

/**
 * @brief JzRE Log Macro
 */
#define JzRE_LOG_INFO(...) \
    JzLogger::GetInstance().Info(std::format(__VA_ARGS__))
#define JzRE_LOG_WARN(...) \
    JzLogger::GetInstance().Warn(std::format(__VA_ARGS__))
#define JzRE_LOG_ERROR(...) \
    JzLogger::GetInstance().Error(std::format(__VA_ARGS__))
#define JzRE_LOG_DEBUG(...) \
    JzLogger::GetInstance().Debug(std::format(__VA_ARGS__))

/**
 * @brief JzRE Singleton logger
 */
class JzLogger {
public:
    /**
     * @brief Get the singleton instance
     *
     * @return
     */
    static JzLogger &GetInstance();

    /**
     * @brief Log a message
     *
     * @param message
     * @param level
     */
    void Log(const String &message, JzELogLevel level);

    /**
     * @brief Log a trace message
     *
     * @param message
     */
    void Trace(const String &message);

    /**
     * @brief Log a debug message
     *
     * @param message
     */
    void Debug(const String &message);

    /**
     * @brief Log an info message
     *
     * @param message
     */
    void Info(const String &message);

    /**
     * @brief Log a warning message
     *
     * @param message
     */
    void Warn(const String &message);

    /**
     * @brief Log an error message
     *
     * @param message
     */
    void Error(const String &message);

    /**
     * @brief Log a critical message
     *
     * @param message
     */
    void Critical(const String &message);

    /**
     * @brief Set the log message callback
     *
     * @param callback
     */
    void SetLogMessageCallback(std::function<void(const JzLogMessage &)> callback);

    /**
     * @brief Clear the log message callback
     */
    void ClearLogMessageCallback();

private:
    JzLogger();
    ~JzLogger()                           = default;
    JzLogger(const JzLogger &)            = delete;
    JzLogger &operator=(const JzLogger &) = delete;

private:
    std::shared_ptr<spdlog::logger>              m_logger;
    std::function<void(const JzLogMessage &)>    m_logCallback;
};

} // namespace JzRE