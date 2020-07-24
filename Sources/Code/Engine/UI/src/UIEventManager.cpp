#include "UIEventManager.hpp"
#include "Core/ETLogger.hpp"
#include "Game/ETGame.hpp"

UIEventManager::UIEventManager() {
}

UIEventManager::~UIEventManager() {
}

bool UIEventManager::init() {
    ETNode<ETUIButtonEventManager>::connect(getEntityId());
    ETNode<ETUIEventManager>::connect(getEntityId());
    ETNode<ETUIViewAppearAnimationEvents>::connect(getEntityId());
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

void UIEventManager::ET_onViewAppeared(EntityId viewId) {
    EntityId activeViewId;
    ET_SendEventReturn(activeViewId, &ETUIViewManager::ET_getActiveViewId);
    if(viewId != activeViewId) {
        return;
    }
    UIViewType viewType = UIViewType::None;
    ET_SendEventReturn(viewType, &ETUIViewManager::ET_getActiveViewType);
    if(viewType == UIViewType::Game) {
        ET_SendEvent(&ETGameStateManager::ET_startGame);
    }
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
            ET_SendEvent(&ETGameStateManager::ET_finishGame);
            ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Game);
            ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Main);
            break;
        }
        case UIEventType::OnGameEnd: {
            if(activeViewType != UIViewType::Game) {
                return;
            }
            ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::EndGame);
            break;
        }
        default: {
        }
    }
}