#include "UI/UIViewSwitcher.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>

UIViewSwitcher::UIViewSwitcher() {
}

UIViewSwitcher::~UIViewSwitcher() {
}

bool UIViewSwitcher::init() {
    ETNode<ETUIViewSwitcher>::connect(getEntityId());
    ETNode<ETAppTimerEvents>::connect(getEntityId());
    ETNode<ETRenderCameraEvents>::connect(getEntityId());
    return true;
}

void UIViewSwitcher::deinit() {
    ETNode<ETAppTimerEvents>::disconnect();
    ETNode<ETUIViewSwitcher>::disconnect();
    ETNode<ETRenderCameraEvents>::disconnect();
}

void UIViewSwitcher::ET_onAppTick(float dt) {
    switch(switchTask.state) {
        case SwitchState::Finished: {
            return;
        }
        case SwitchState::HideOldView: {
            bool isAnimEnded = true;
            switchTask.duration += dt;
            ET_SendEventReturn(isAnimEnded, switchTask.oldViewId, &ETUIAppearAnimation::ET_animate, switchTask.duration);
            if(!isAnimEnded) {
                break;
            } else {
                switchTask.state = SwitchState::ShowNewView;
                float duration = 0.f;
                ET_SendEventReturn(duration, switchTask.oldViewId, &ETUIAppearAnimation::ET_getDuration);
                switchTask.duration -= duration;
                if(switchTask.duration < 0.01f) {
                    break;
                } else {
                    ET_SendEvent(switchTask.oldViewId, &ETUIVisibleElement::ET_hide);
                    [[fallthrough]];
                }
            }
        }
        case SwitchState::ShowNewView: {
            bool isVisible = false;
            ET_SendEventReturn(isVisible, switchTask.newViewId, &ETUIVisibleElement::ET_isVisible);
            if(!isVisible) {
                ET_SendEvent(switchTask.newViewId, &ETUIVisibleElement::ET_show);
            }
            bool isAnimEnded = true;
            switchTask.duration += dt;
            ET_SendEventReturn(isAnimEnded, switchTask.newViewId, &ETUIAppearAnimation::ET_animate, switchTask.duration);
            if(isAnimEnded) {
                switchTask.state = SwitchState::Finished;
                ET_SendEvent(&ETUIViewSwitcherEvents::ET_onViewSwitchFinished, switchTask.newViewId);
            }
            break;
        }
        default: {
            assert(false && "Invalid switch task state");
        }
    }
}

void UIViewSwitcher::ET_reverseSwitchView(EntityId newViewId, EntityId oldViewId) {
    ET_swtichView(oldViewId, newViewId);
}

void UIViewSwitcher::ET_swtichView(EntityId newViewId, EntityId oldViewId) {
    assert(newViewId.isValid() && "Invalid new view");

    bool isVisible = false;
    ET_SendEventReturn(isVisible, newViewId, &ETUIVisibleElement::ET_isVisible);
    if(isVisible) {
        ET_SendEvent(newViewId, &ETUIVisibleElement::ET_hide);
    }

    switchTask.state = SwitchState::ShowNewView;
    if(oldViewId.isValid()) {
        bool hideOldView = true;
        ET_SendEventReturn(hideOldView, newViewId, &ETUIAppearAnimation::ET_isNeedHideOldView);
        if(hideOldView) {
            switchTask.state = SwitchState::HideOldView;
            switchTask.oldViewId = oldViewId;
            ET_SendEvent(oldViewId, &ETUIAppearAnimation::ET_setAppear, false);
        }
    }

    ET_SendEvent(newViewId, &ETUIAppearAnimation::ET_setAppear, true);
    switchTask.newViewId = newViewId;
    switchTask.duration = 0.f;
}

void UIViewSwitcher::ET_forceSwtichStop() {
    switchTask.oldViewId = InvalidEntityId;
    switchTask.newViewId = InvalidEntityId;
    switchTask.state = SwitchState::Finished;
    switchTask.duration = 0.f;
}

void UIViewSwitcher::ET_reverse() {
    switch(switchTask.state) {
        case SwitchState::HideOldView: {
            ET_SendEvent(switchTask.oldViewId, &ETUIAppearAnimation::ET_setAppear, true);

            float origDuration = 0.f;
            ET_SendEventReturn(origDuration, switchTask.oldViewId, &ETUIAppearAnimation::ET_getDuration);
            switchTask.duration = origDuration - switchTask.duration;

            switchTask.state = SwitchState::ShowNewView;
            std::swap(switchTask.newViewId, switchTask.oldViewId);
            break;
        }
        case SwitchState::ShowNewView: {
            ET_SendEvent(switchTask.newViewId, &ETUIAppearAnimation::ET_setAppear, false);
            ET_SendEvent(switchTask.oldViewId, &ETUIAppearAnimation::ET_setAppear, true);

            float origDuration = 0.f;
            ET_SendEventReturn(origDuration, switchTask.newViewId, &ETUIAppearAnimation::ET_getDuration);
            switchTask.duration = origDuration - switchTask.duration;

            switchTask.state = SwitchState::HideOldView;
            std::swap(switchTask.newViewId, switchTask.oldViewId);
            break;
        }
        case SwitchState::Finished: {
            LogError("[UIViewSwitcher::ET_reverse] Can't do reverse switch without active one");
            return;
        }
        default: {
            assert(false && "Invalid switch state");
        }
    }
}

void UIViewSwitcher::ET_onRenderPortResized() {
    switch(switchTask.state) {
        case SwitchState::HideOldView: {
            float duration = 0.f;
            ET_SendEventReturn(duration, switchTask.oldViewId, &ETUIAppearAnimation::ET_getDuration);
            duration += 0.1f;
            ET_SendEvent(switchTask.oldViewId, &ETUIAppearAnimation::ET_animate, duration);
            switchTask.state = SwitchState::ShowNewView;
            [[fallthrough]];
        }
        case SwitchState::ShowNewView: {
            float duration = 0.f;
            ET_SendEventReturn(duration, switchTask.newViewId, &ETUIAppearAnimation::ET_getDuration);
            duration += 0.1f;
            ET_SendEvent(switchTask.newViewId, &ETUIAppearAnimation::ET_animate, duration);
            ET_SendEvent(&ETUIViewSwitcherEvents::ET_onViewSwitchFinished, switchTask.newViewId);
            switchTask.state = SwitchState::Finished;
            [[fallthrough]];
        }
        case SwitchState::Finished: {
            return;
        }
        default: {
            assert(false && "Invalid switch task state");
        }
    }
}

float UIViewSwitcher::ET_getTotalSwitchDuration() const {
    if(switchTask.state == SwitchState::Finished) {
        LogWarning("[UIViewSwitcher::ET_getTotalSwitchDuration] Can't get total duration without active switch task");
        return 0.f;
    }
    float totalDuration = 0.f;
    if(switchTask.oldViewId.isValid()) {
        float duration = 0.f;
        ET_SendEventReturn(duration, switchTask.oldViewId, &ETUIAppearAnimation::ET_getDuration);
        totalDuration += duration;
    }
    if(switchTask.newViewId.isValid()) {
        float duration = 0.f;
        ET_SendEventReturn(duration, switchTask.newViewId, &ETUIAppearAnimation::ET_getDuration);
        totalDuration += duration;
    }
    return totalDuration;
}
