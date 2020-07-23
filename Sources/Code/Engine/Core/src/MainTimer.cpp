#include "MainTimer.hpp"

#include <cassert>

MainTimer::MainTimer() :
    appTimeScale(1.f) {
}

MainTimer::~MainTimer() {
}

bool MainTimer::init() {
    frameStartT = TimePoint::GetNowTime();
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
    auto currT = TimePoint::GetNowTime();

    auto tickDuration = currT.getSecondsElapsedFrom(frameStartT);
    frameStartT = currT;

    ET_SendEvent(&ETSystemTimerEvents::ET_onSystemTick, tickDuration);

    auto appTickDuration = tickDuration * appTimeScale;
    ET_SendEvent(&ETAppTimerEvents::ET_onAppTick, appTickDuration);
}

TimePoint MainTimer::ET_getFrameStartTime() const {
    return frameStartT;
}