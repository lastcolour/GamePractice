#include "UITimer.hpp"
#include "UI/ETUITimer.hpp"
#include "UI/ETUIViewCache.hpp"
#include "UI/ETUIViewPort.hpp"
#include "UI/ETUIBox.hpp"
#include "Render/ETRenderTickManager.hpp"

UITimer::UITimer() :
    scale(1.f),
    skipUpdate(false),
    isPaused(false) {
}

UITimer::~UITimer() {
}

bool UITimer::init() {
    ETNode<ETUITimer>::connect(getEntityId());
    return true;
}

void UITimer::deinit() {
}

void UITimer::ET_onTick(float dt) {
    ET_PollAllEvents<ETUITimer>();
    ET_PollAllEvents<ETUIViewCache>();

    if(isPaused) {
        ET_SendEvent(&ETUIReAlignManager::ET_doReAlign);
        return;
    }

    if(skipUpdate) {
        dt = 0.f;
        skipUpdate = false;
    }

    dt *= scale;

    ET_SendEvent(&ETRenderTickManager::ET_onUITick, dt);
    ET_SendEvent(&ETUITimerEvents::ET_onUITick, dt);

    ET_SendEvent(&ETUIReAlignManager::ET_doReAlign);

    ET_SendEvent(&ETUIAdditiveAnimationTarget::ET_applyAdditiveTranform);
    ET_SendEvent(&ETUIAdditiveAnimationTarget::ET_resetAdditiveTransform);
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