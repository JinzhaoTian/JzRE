/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzLogSink.h"
#include "spdlog/common.h"

template <typename Mutex>
void JzRE::JzLogSink<Mutex>::sink_it_(const spdlog::details::log_msg &msg)
{
    // format the message using the formatter
    spdlog::memory_buf_t formatted;
    spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);

    // create JzLogMessage object
    JzLogMessage logMsg;
    logMsg.message      = fmt::to_string(formatted);
    logMsg.sourceFile   = msg.source.filename;
    logMsg.line         = msg.source.line;
    logMsg.functionName = msg.source.funcname;
    logMsg.threadId     = msg.thread_id;
    logMsg.level        = [&]() {
        switch (msg.level) {
            case spdlog::level::trace:
                return JzELogLevel::Trace;
            case spdlog::level::debug:
                return JzELogLevel::Debug;
            case spdlog::level::info:
                return JzELogLevel::Info;
            case spdlog::level::warn:
                return JzELogLevel::Warning;
            case spdlog::level::err:
                return JzELogLevel::Error;
            case spdlog::level::critical:
                return JzELogLevel::Critical;
            default:
                return JzELogLevel::Info;
        }
    }();

    m_event.Invoke(logMsg);
}

template <typename Mutex>
void JzRE::JzLogSink<Mutex>::flush_()
{
    // Implement any necessary flushing logic
    // This is called when spdlog::logger::flush() is invoked
}

template class JzRE::JzLogSink<std::mutex>;
