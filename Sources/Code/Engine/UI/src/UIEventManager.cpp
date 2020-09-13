#include "UIEventManager.hpp"
#include "Core/ETLogger.hpp"
#include "Game/ETGame.hpp"

UIEventManager::UIEventManager() {
}

UIEventManager::~UIEventManager() {
}

bool UIEventManager::init() {
    initEventHandlers();

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
            ET_SendEvent(&ETGameStateManager::ET_pauseGame);
            ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::PauseGame);
            break;
        }
        case UIViewType::PauseGame: {
            ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::PauseGame);
            ET_SendEvent(&ETGameStateManager::ET_resumeGame);
            break;
        }
        default: {
        }
    }
}

void UIEventManager::initEventHandlers() {
    eventHandlers[UIEventType::OnBackButton];
    eventHandlers[UIEventType::OnMainViewStartGame] = [](UIViewType activeView){
        if(activeView != UIViewType::Main) {
            LogError("[UIEventManager::ET_onEvent] Can't start game from non-main view");
            return;
        }
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Main);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Game);
    };
    eventHandlers[UIEventType::OnGameEndViewExit] = [](UIViewType activeView){
        if(activeView != UIViewType::EndGame) {
            LogError("[UIEventType::ET_onEvent] Can't trigger exit from game end view from non-game view");
            return;
        }
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::EndGame);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Main);
    };
    eventHandlers[UIEventType::OnBackButton] = [this](UIViewType activeView){
        handleBackButtonEvent(activeView);
    };
    eventHandlers[UIEventType::OnGameGameEnd] = [](UIViewType activeView){
        if(activeView != UIViewType::Game) {
            LogError("[UIEventType::ET_onEvent] Can't trigger game end from non-game end view");
            return;
        }
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Game);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::EndGame);
    };
    eventHandlers[UIEventType::OnSurfaceHidden] = [](UIViewType activeView){

    };
    eventHandlers[UIEventType::OnSurfaceShown] = [](UIViewType activeView){

    };
    eventHandlers[UIEventType::OnPauseViewResume] = [](UIViewType activeView){
        if(activeView != UIViewType::PauseGame) {
            LogError("[UIEventType::ET_onEvent] Can't resume game from non-pause view");
            return;
        }
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::PauseGame);
        ET_SendEvent(&ETGameStateManager::ET_resumeGame);
    };
    eventHandlers[UIEventType::OnPauseViewRestart] = [](UIViewType activeView){
        if(activeView != UIViewType::PauseGame) {
            LogError("[UIEventType::ET_onEvent] Can't restart game from non-pause view");
            return;
        }
        ET_SendEvent(&ETGameStateManager::ET_finishGame);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Game);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::PauseGame);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Game);
    };
    eventHandlers[UIEventType::OnPauseViewExit] = [](UIViewType activeView){
        if(activeView != UIViewType::PauseGame) {
            LogError("[UIEventType::ET_onEvent] Can't exit game from non-pause view");
            return;
        }
        ET_SendEvent(&ETGameStateManager::ET_finishGame);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::PauseGame);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Game);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Main);
    };
}

void UIEventManager::ET_onEvent(UIEventType eventType) {
    bool transitionInProgress = false;
    ET_SendEventReturn(transitionInProgress, &ETUIViewTransitionManager::ET_hasActiveTransition);
    if(transitionInProgress) {
        return;
    }

    auto it = eventHandlers.find(eventType);
    if(it == eventHandlers.end()) {
        return;
    }

    UIViewType activeViewType = UIViewType::None;
    ET_SendEventReturn(activeViewType, &ETUIViewManager::ET_getActiveViewType);

    auto& callF = it->second;
    callF(activeViewType);
}