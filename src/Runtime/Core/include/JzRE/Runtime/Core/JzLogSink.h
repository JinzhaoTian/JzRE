/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <functional>
#include "spdlog/common.h"
#include "spdlog/details/log_msg.h"
#include "spdlog/sinks/base_sink.h"
#include "JzELog.h"

namespace JzRE {

/**
 * @brief JzRE event log sink
 *
 * @tparam Mutex
 */
template <typename Mutex>
class JzLogSink : public spdlog::sinks::base_sink<Mutex> {
public:
    /**
     * @brief Constructor
     *
     * @param callback
     */
    explicit JzLogSink(std::function<void(const JzLogMessage &)> &callback) :
        m_callback(callback) { }

protected:
    void sink_it_(const spdlog::details::log_msg &msg) override
    {
        // format the message using the formatter
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);

        // create JzLogMessage object
        JzLogMessage logMsg;
        logMsg.message = fmt::to_string(formatted);
        // logMsg.sourceFile   = msg.source.filename;
        // logMsg.line         = msg.source.line;
        // logMsg.functionName = msg.source.funcname;
        // logMsg.threadId     = msg.thread_id;
        logMsg.level = [&]() {
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

        if (m_callback) m_callback(logMsg);
    }

    void flush_() override
    {
        // Implement any necessary flushing logic
        // This is called when spdlog::logger::flush() is invoked
    }

private:
    std::function<void(const JzLogMessage &)> &m_callback;
};

} // namespace JzRE