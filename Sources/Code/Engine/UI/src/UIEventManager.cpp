#include "UIEventManager.hpp"
#include "Core/ETLogger.hpp"

UIEventManager::UIEventManager() {
}

UIEventManager::~UIEventManager() {
}

bool UIEventManager::init() {
    ETNode<ETUIButtonEventManager>::connect(getEntityId());
    ETNode<ETUIEventManager>::connect(getEntityId());
    return true;
}

void UIEventManager::deinit() {
}

EntityId UIEventManager::ET_getActiveButton() const {
    return activeButtonId;
}

void UIEventManager::ET_setActiveButton(EntityId buttonId) {
    activeButtonId = buttonId;
}

void UIEventManager::ET_onEvent(UIEventType eventType) {
    bool transitionInProgress = false;
    ET_SendEventReturn(transitionInProgress, &ETUIViewTransitionManager::ET_hasActiveTransition);
    if(transitionInProgress) {
        return;
    }

    UIViewType activeViewType = UIViewType::None;
    ET_SendEventReturn(activeViewType, &ETUIViewManager::ET_getActiveViewType);

    switch(eventType) {
        case UIEventType::OnStartGame: {
            if(activeViewType != UIViewType::Main) {
                LogError("[UIEventManager::ET_onEvent] Can't start game from non-main view");
                return;
            }
            ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Main);
            ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Game);
            break;
        }
        case UIEventType::OnBackButton: {
            if(activeViewType != UIViewType::Game) {
                return;
            }
            ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Game);
            ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Main);
        }
        default: {
        }
    }
}