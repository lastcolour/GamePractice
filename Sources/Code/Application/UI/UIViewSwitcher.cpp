#include "UI/UIViewSwitcher.hpp"

#include <cassert>

UIViewSwitcher::UIViewSwitcher() {
}

UIViewSwitcher::~UIViewSwitcher() {
}

bool UIViewSwitcher::init() {
    ETNode<ETUIViewSwitcher>::connect(getEntityId());
    ETNode<ETTimerEvents>::connect(getEntityId());
    ETNode<ETRenderEvents>::connect(getEntityId());
    return true;
}

void UIViewSwitcher::deinit() {
    ETNode<ETTimerEvents>::disconnect();
    ETNode<ETUIViewSwitcher>::disconnect();
    ETNode<ETRenderEvents>::disconnect();
}

void UIViewSwitcher::ET_onTick(float dt) {
    switch(switchTask.state) {
        case SwitchState::Finished: {
            return;
        }
        case SwitchState::HideOldView: {
            bool isAnimEnded = true;
            ET_SendEventReturn(isAnimEnded, switchTask.oldViewId, &ETUIAppearAnimation::ET_animate, switchTask.duration);
            switchTask.duration += dt;
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
                    // Process 'ShowNewView'
                }
            }
        }
        case SwitchState::ShowNewView: {
            bool isAnimEnded = true;
            ET_SendEventReturn(isAnimEnded, switchTask.newViewId, &ETUIAppearAnimation::ET_animate, switchTask.duration);
            switchTask.duration += dt;
            if(isAnimEnded) {
                ET_SendEvent(&ETUIViewSwitcherEvents::ET_onViewSwitchFinished, switchTask.newViewId);
                switchTask.state = SwitchState::Finished;
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

    if(oldViewId.isValid()) {
        bool hideOldView = true;
        ET_SendEventReturn(hideOldView, newViewId, &ETUIAppearAnimation::ET_isNeedHideOldView);
        if(hideOldView) {
            switchTask.state = SwitchState::HideOldView;
            switchTask.oldViewId = oldViewId;
            ET_SendEvent(oldViewId, &ETUIAppearAnimation::ET_setAppear, false);

        } else {
            switchTask.oldViewId = InvalidEntityId;
            switchTask.state = SwitchState::ShowNewView;
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
        }
        case SwitchState::ShowNewView: {
            float duration = 0.f;
            ET_SendEventReturn(duration, switchTask.newViewId, &ETUIAppearAnimation::ET_getDuration);
            duration += 0.1f;
            ET_SendEvent(switchTask.newViewId, &ETUIAppearAnimation::ET_animate, duration);
            switchTask.state = SwitchState::Finished;
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
