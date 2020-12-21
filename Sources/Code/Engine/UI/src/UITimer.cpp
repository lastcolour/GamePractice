#include "UITimer.hpp"
#include "UI/ETUITimer.hpp"
#include "UI/ETUIViewCache.hpp"
#include "UI/ETUIViewPort.hpp"
#include "UI/ETUIBox.hpp"

UITimer::UITimer() :
    scale(1.f),
    skipUpdate(false),
    isPaused(false) {
}

UITimer::~UITimer() {
}

bool UITimer::init() {
    lastTickT = TimePoint::GetNowTime();
    ETNode<ETUITimer>::connect(getEntityId());
    return true;
}

void UITimer::deinit() {
}

void UITimer::ET_onTick() {
    ET_PollAllEvents<ETUITimer>();
    ET_PollAllEvents<ETUIViewCache>();

    if(isPaused) {
        return;
    }
    auto currTime = TimePoint::GetNowTime();
    auto dt = currTime.getSecElapsedFrom(lastTickT);
    lastTickT = currTime;

    if(skipUpdate) {
        dt = 0.f;
        skipUpdate = false;
    }

    dt *= scale;
    ET_SendEvent(&ETUITimerEvents::ET_onUITick, dt);
    ET_SendEvent(&ETUIAdditiveAnimationTarget::ET_applyAdditiveTranform);
}

void UITimer::ET_setScale(float newScale) {
    scale = newScale;
}

void UITimer::ET_pause() {
    isPaused = true;
}

void UITimer::ET_resume() {
    isPaused = false;
    skipUpdate = true;
}

bool UITimer::ET_isPaused() const {
    return isPaused;
}