#pragma once

#include <chrono>
#include "JzRETypes.h"

namespace JzRE {
/**
 * @brief Clock
 */
class JzClock {
public:
    /**
     * @brief Update the clock
     */
    void Update();

    /**
     * @brief Get the frame rate
     */
    F32 GetFrameRate();

    /**
     * @brief Get the delta time
     */
    F32 GetDeltaTime();

    /**
     * @brief Get the unscaled delta time
     */
    F32 GetDeltaTimeUnScale();

    /**
     * @brief Get the time since start
     */
    F32 GetTimeSinceStart();

    /**
     * @brief Get the time scale
     */
    F32 GetTimeScale();

    /**
     * @brief Scale the time
     */
    void Scale(F32 coefficient);

    /**
     * @brief Set the time scale
     */
    void SetTimeScale(F32 timeScale);

private:
    /**
     * @brief Initialize the clock
     */
    void Initialize();

    std::chrono::steady_clock::time_point __START_TIME;
    std::chrono::steady_clock::time_point __CURRENT_TIME;
    std::chrono::steady_clock::time_point __LAST_TIME;
    std::chrono::duration<F64>            __ELAPSED;

    Bool __INITIALIZED      = false;
    F32  __TIME_SCALE       = 1.0f;
    F32  __DELTA_TIME       = 0.0f;
    F32  __TIME_SINCE_START = 0.0f;
};
} // namespace JzRE