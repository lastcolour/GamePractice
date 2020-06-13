#include "Timer.hpp"

namespace {

const float MIN_TICK_DURATION = 0.001f;

} // namespace

Timer::Timer() :
    lastTickT(),
    tickDuration(0.f) {
}

Timer::~Timer() {
}

bool Timer::init() {
    lastTickT = ClockT::now();
    ETNode<ETTimer>::connect(getEntityId());
    return true;
}

void Timer::deinit() {
    ETNode<ETTimer>::disconnect();
}

void Timer::ET_onFrameStart() {
    auto timeNow = ClockT::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>
        (timeNow - lastTickT).count();
    tickDuration = static_cast<float>(timeDiff / 1000.f);

    if(tickDuration > 0.001f) {
        lastTickT = timeNow;
        ET_SendEvent(&ETTimerEvents::ET_onTick, tickDuration);
    }
}