#include "UITimer.hpp"
#include "UI/ETUITimer.hpp"

UITimer::UITimer() {
}

UITimer::~UITimer() {
}

bool UITimer::init() {
    ETNode<ETUIUpdateTask>::connect(getEntityId());
    return true;
}

void UITimer::deinit() {
}

void UITimer::ET_updateUI() {
    float dt = 0.f;
    ET_SendEvent(&ETUITimerEvents::ET_onUITick, dt);
}