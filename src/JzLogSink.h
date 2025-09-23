
/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "spdlog/details/log_msg.h"
#include "spdlog/sinks/base_sink.h"
#include "JzEvent.h"
#include "JzELog.h"

namespace JzRE {

/**
 * @brief JzRE Log Sink
 *
 * @tparam Mutex
 */
template <typename Mutex>
class JzLogSink : public spdlog::sinks::base_sink<Mutex> {
public:
    /**
     * @brief Constructor
     *
     * @param event
     */
    explicit JzLogSink(JzEvent<const JzLogMessage &> &event) :
        m_event(event) { }

    /**
     * @brief Get the Event object
     *
     * @return JzEvent<const JzLogMessage &>&
     */
    JzEvent<const JzLogMessage &> &GetEvent()
    {
        return m_event;
    }

protected:
    void sink_it_(const spdlog::details::log_msg &msg) override;
    void flush_() override;

private:
    JzEvent<const JzLogMessage &> &m_event;
};

} // namespace JzRE
