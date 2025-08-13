#include "JzClock.h"

void JzRE::JzClock::Initialize()
{
    __DELTA_TIME = 0.0f;

    __START_TIME   = std::chrono::steady_clock::now();
    __CURRENT_TIME = __START_TIME;
    __LAST_TIME    = __START_TIME;

    __INITIALIZED = true;
}

void JzRE::JzClock::Update()
{
    __LAST_TIME    = __CURRENT_TIME;
    __CURRENT_TIME = std::chrono::steady_clock::now();
    __ELAPSED      = __CURRENT_TIME - __LAST_TIME;

    if (__INITIALIZED) {
        __DELTA_TIME = __ELAPSED.count() > 0.1f ? 0.1f : static_cast<F32>(__ELAPSED.count());
        __TIME_SINCE_START += __DELTA_TIME * __TIME_SCALE;
    } else {
        Initialize();
    }
}

JzRE::F32 JzRE::JzClock::GetFrameRate()
{
    return 1.0f / (__DELTA_TIME);
}

JzRE::F32 JzRE::JzClock::GetDeltaTime()
{
    return __DELTA_TIME * __TIME_SCALE;
}

JzRE::F32 JzRE::JzClock::GetDeltaTimeUnScale()
{
    return __DELTA_TIME;
}

JzRE::F32 JzRE::JzClock::GetTimeSinceStart()
{
    return __TIME_SINCE_START;
}

JzRE::F32 JzRE::JzClock::GetTimeScale()
{
    return __TIME_SCALE;
}

void JzRE::JzClock::Scale(F32 coefficient)
{
    __TIME_SCALE *= coefficient;
}

void JzRE::JzClock::SetTimeScale(F32 timeScale)
{
    __TIME_SCALE = timeScale;
}
