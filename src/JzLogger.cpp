/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzLogger.h"
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "JzELog.h"
#include "JzLogSink.h"

JzRE::JzLogger::JzLogger()
{
    try {
        std::vector<spdlog::sink_ptr> sinks;

        // console sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        sinks.push_back(console_sink);

        // file sink
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/app.log", 1048576 * 5, 3);
        sinks.push_back(file_sink);

        // custom event sink
        auto event_sink = std::make_shared<JzLogSink<std::mutex>>(OnLogMessage);
        sinks.push_back(event_sink);

        // create multiple sink logger
        m_logger = std::make_shared<spdlog::logger>("main_logger", sinks.begin(), sinks.end());

        // log format
        m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

        // log level
        m_logger->set_level(spdlog::level::info);
        m_logger->flush_on(spdlog::level::info);

        spdlog::register_logger(m_logger);
    } catch (const spdlog::spdlog_ex &ex) {
        spdlog::stderr_color_mt("stderr");
        spdlog::get("stderr")->error("日志初始化失败: {}", ex.what());
    }
}

JzRE::JzLogger &JzRE::JzLogger::GetInstance()
{
    static JzLogger instance;
    return instance;
}

void JzRE::JzLogger::Log(const JzRE::String &message, JzRE::JzELogLevel level)
{
    // system log
    m_logger->log([&]() {
        switch (level) {
            case JzELogLevel::Trace:
                return spdlog::level::trace;
            case JzELogLevel::Debug:
                return spdlog::level::debug;
            case JzELogLevel::Info:
                return spdlog::level::info;
            case JzELogLevel::Warning:
                return spdlog::level::warn;
            case JzELogLevel::Error:
                return spdlog::level::err;
            case JzELogLevel::Critical:
                return spdlog::level::critical;
        }
    }(),
                  message);
}

void JzRE::JzLogger::Trace(const JzRE::String &message)
{
    Log(message, JzELogLevel::Trace);
}

void JzRE::JzLogger::Debug(const JzRE::String &message)
{
    Log(message, JzELogLevel::Debug);
}

void JzRE::JzLogger::Info(const JzRE::String &message)
{
    Log(message, JzELogLevel::Info);
}

void JzRE::JzLogger::Warn(const JzRE::String &message)
{
    Log(message, JzELogLevel::Warning);
}

void JzRE::JzLogger::Error(const JzRE::String &message)
{
    Log(message, JzELogLevel::Error);
}

void JzRE::JzLogger::Critical(const JzRE::String &message)
{
    Log(message, JzELogLevel::Critical);
}
