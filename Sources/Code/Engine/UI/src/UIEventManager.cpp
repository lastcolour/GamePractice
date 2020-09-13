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

void UIEventManager::handleBackButtonEvent(UIViewType activeViewType) {
    switch(activeViewType) {
        case UIViewType::EndGame: {
            ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::EndGame);
            ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Main);
            break;
        }
        case UIViewType::Game: {
            ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Game);
            ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::EndGame);
            break;
        }
        default: {
        }
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
            handleBackButtonEvent(activeViewType);
            break;
        }
        case UIEventType::OnGameEnd: {
            if(activeViewType != UIViewType::Game) {
                LogError("[UIEventType::ET_onEvent] Can't trigger game end from non-game view");
                return;
            }
            ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::EndGame);
            break;
        }
        case UIEventType::OnRestartButton: {
            if(activeViewType != UIViewType::EndGame) {
                LogError("[UIEventType::ET_onEvent] Can't restart from non game-end view");
                return;
            }
            ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::EndGame);
            ET_onEvent(UIEventType::OnStartGame);
            break;
        }
        case UIEventType::OnExitGameEndViewButton: {
            if(activeViewType != UIViewType::EndGame) {
                LogError("[UIEventType::ET_onEvent] Can't exit from non game-end view");
                return;
            }
            ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::EndGame);
            ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Main);
            break;
        }
        case UIEventType::OnSurfaceShown: {
            break;
        }
        default: {
            LogError("[UIEventManager::ET_onEvent] Unknown event type: '%d'", eventType);
        }
    }
}