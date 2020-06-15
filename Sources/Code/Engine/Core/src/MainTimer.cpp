#include "MainTimer.hpp"

#include <cassert>

MainTimer::MainTimer() :
    lastTickT(),
    appTimeScale(1.f) {
}

MainTimer::~MainTimer() {
}

bool MainTimer::init() {
    lastTickT = ClockT::now();
    ETNode<ETMainThreadTimer>::connect(getEntityId());
    return true;
}

void MainTimer::deinit() {
    ETNode<ETMainThreadTimer>::disconnect();
}

void MainTimer::ET_setAppTimeScale(float newScale) {
    assert(newScale > 0.f);
    appTimeScale = newScale;
}

void MainTimer::ET_onMainThreadStep() {
    auto timeNow = ClockT::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastTickT).count();
    auto tickDuration = static_cast<float>(timeDiff / 1000.f);
    lastTickT = timeNow;

    ET_SendEvent(&ETSystemTimerEvents::ET_onSystemTick, tickDuration);

    auto appTickDuration = tickDuration * appTimeScale;
    ET_SendEvent(&ETAppTimerEvents::ET_onAppTick, appTickDuration);
}