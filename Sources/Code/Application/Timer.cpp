#include "Timer.hpp"

#include <chrono>

struct TimePoint {
public:

    typedef typename std::chrono::high_resolution_clock ClockT;
    typedef typename std::chrono::time_point<ClockT> TimePointT;

public:

    TimePointT lastTickT;
    float tickDuration;
};

Timer::Timer() :
    timePoint(new TimePoint) {
    timePoint->tickDuration = 0;
}

Timer::~Timer() {
}

bool Timer::init() {
    ETNode<ETTimer>::connect(getEntityId());
    return true;
}

void Timer::deinit() {
    ETNode<ETTimer>::disconnect();
}

void Timer::ET_onFrameStart() {
    auto timeNow = TimePoint::ClockT::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>
        (timeNow - timePoint->lastTickT).count();
    timePoint->lastTickT = timeNow;
    timePoint->tickDuration = static_cast<float>(timeDiff / 1000.f);

    ET_SendEvent(&ETTimerEvents::ET_onTick, timePoint->tickDuration);
}