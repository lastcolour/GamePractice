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

float Timer::getFrameTime() const {
    return timePoint->tickDuration;
}

float Timer::tick() {
    auto timeNow = TimePoint::ClockT::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>
        (timeNow - timePoint->lastTickT).count();
    timePoint->lastTickT = timeNow;
    timePoint->tickDuration = timeDiff / 1000.f;
    return timePoint->tickDuration;
}