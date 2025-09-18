/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/base_sink.h"
#include "JzRETypes.h"
#include "JzEvent.h"

namespace JzRE {

class JzLogger;

enum class JzELogLevel {
    Info,
    Warning,
    Error,
    Debug
};

struct JzLogMessage {
    String      message;
    JzELogLevel level;
};

template <typename Mutex>
class JzLogSink : public spdlog::sinks::base_sink<Mutex> {
public:
    JzLogSink(JzLogger &logger) :
        m_logger(logger) { }

protected:
    void sink_it_(const spdlog::details::log_msg &msg) override;

    void flush_() override { }

private:
    JzLogger &m_logger;
};

/**
 * @brief A singleton logger class
 */
class JzLogger {
public:
    /**
     * @brief Get the singleton instance
     * @return
     */
    static JzLogger &GetInstance();

    /**
     * @brief Log a message
     * @param message
     * @param level
     */
    void Log(const String &message, JzELogLevel level = JzELogLevel::Info);

    /**
     * @brief Log an info message
     * @param message
     */
    void Info(const String &message);

    /**
     * @brief Log a warning message
     * @param message
     */
    void Warn(const String &message);

    /**
     * @brief Log an error message
     * @param message
     */
    void Error(const String &message);

    /**
     * @brief Log a debug message
     * @param message
     */
    void Debug(const String &message);

    /**
     * @brief Get all log messages
     * @return
     */
    const std::vector<JzLogMessage> &GetMessages() const;

    /**
     * @brief Clear all log messages
     */
    void Clear();

    JzEvent<const JzLogMessage &> OnLogMessage;

private:
    JzLogger();
    ~JzLogger()                           = default;
    JzLogger(const JzLogger &)            = delete;
    JzLogger &operator=(const JzLogger &) = delete;

    std::vector<JzLogMessage>       m_messages;
    std::shared_ptr<spdlog::logger> m_logger;
};
} // namespace JzRE