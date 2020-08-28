#include "UITimer.hpp"
#include "UI/ETUITimer.hpp"
#include "UI/ETUIView.hpp"

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
    ET_PollAllEvents<ETUIViewManager>();

    auto currTime = TimePoint::GetNowTime();
    auto dt = currTime.getSecondsElapsedFrom(lastTickT);
    lastTickT = currTime;
    ET_SendEvent(&ETUITimerEvents::ET_onUITick, dt);
}