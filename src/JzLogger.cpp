/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzLogger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

template <typename Mutex>
void JzRE::JzLogSink<Mutex>::sink_it_(const spdlog::details::log_msg &msg)
{
    spdlog::memory_buf_t formatted;
    spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
    m_logger.Log(fmt::to_string(formatted));
}

JzRE::JzLogger::JzLogger()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] [%l] %v%$");

    auto logSink = std::make_shared<JzLogSink<std::mutex>>(*this);
    logSink->set_pattern("[%H:%M:%S.%e] [%l] %v");

    m_logger = std::make_shared<spdlog::logger>("JzRE", spdlog::sinks_init_list{console_sink, logSink});
    m_logger->set_level(spdlog::level::trace);
}

JzRE::JzLogger &JzRE::JzLogger::GetInstance()
{
    static JzLogger instance;
    return instance;
}

void JzRE::JzLogger::Log(const JzRE::String &message, JzRE::JzELogLevel level)
{
    JzLogMessage logMessage{message, level};

    if (message.find("[info]") != String::npos) {
        level = JzELogLevel::Info;
    } else if (message.find("[warning]") != String::npos) {
        level = JzELogLevel::Warning;
    } else if (message.find("[error]") != String::npos) {
        level = JzELogLevel::Error;
    } else if (message.find("[debug]") != String::npos) {
        level = JzELogLevel::Debug;
    }

    m_messages.push_back(logMessage);
    OnLogMessage.Invoke(logMessage);
}

void JzRE::JzLogger::Info(const JzRE::String &message)
{
    m_logger->info(message);
}

void JzRE::JzLogger::Warn(const JzRE::String &message)
{
    m_logger->warn(message);
}

void JzRE::JzLogger::Error(const JzRE::String &message)
{
    m_logger->error(message);
}

void JzRE::JzLogger::Debug(const JzRE::String &message)
{
    m_logger->debug(message);
}

const std::vector<JzRE::JzLogMessage> &JzRE::JzLogger::GetMessages() const
{
    return m_messages;
}

void JzRE::JzLogger::Clear()
{
    m_messages.clear();
}