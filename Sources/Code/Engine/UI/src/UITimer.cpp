#include "UITimer.hpp"
#include "UI/ETUITimer.hpp"

UITimer::UITimer() {
}

UITimer::~UITimer() {
}

bool UITimer::init() {
    lastTickT = TimePoint::GetNowTime();
    ETNode<ETUIUpdateTask>::connect(getEntityId());
    return true;
}

void UITimer::deinit() {
}

void UITimer::ET_updateUI() {
    auto currTime = TimePoint::GetNowTime();
    auto dt = currTime.getSecondsElapsedFrom(lastTickT);
    lastTickT = currTime;
    ET_SendEvent(&ETUITimerEvents::ET_onUITick, dt);
}