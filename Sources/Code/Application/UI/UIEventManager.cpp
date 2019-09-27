#include "UI/UIEventManager.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Entity/ETEntityInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"

UIEventManager::UIEventManager() :
    isButtonPressed(false) {

    viewMap["UI/GameView/Root.json"] = EViewType::Game;
    viewMap["UI/PlayerView/Root.json"] = EViewType::Player;
    viewMap["UI/AboutView/Root.json"] = EViewType::About;
    viewMap["UI/OptionsView/Root.json"] = EViewType::Options;
    viewMap["UI/EndGameView/Root.json"] = EViewType::EndGame;
    viewMap["UI/MainView/Root.json"] = EViewType::Main;
    viewMap["UI/ExitView/Root.json"] = EViewType::Exit;
    viewMap["UI/PauseView/Root.json"] = EViewType::Pause;

    setupCallbacks();
}

UIEventManager::~UIEventManager() {
}

void UIEventManager::setupCallbacks() {
    eventMap["Main_OnStartButton"] = [this](){
        pushView(EViewType::Game);
    };
    eventMap["Main_OnPlayerButton"] = [this](){
        pushView(EViewType::Player);
    };
    eventMap["Main_OnAboutButton"] = [this](){
        pushView(EViewType::About);
    };
    eventMap["Main_OnOptionsButton"] = [this](){
        pushView(EViewType::Options);
    };
    eventMap["Game_OnGameEnd"] = [this](){
        pushView(EViewType::EndGame);
    };
    eventMap["EndGame_OnExitToMainButton"] = [this](){
        pushView(EViewType::Main);
    };
    eventMap["EndGame_OnPlayAgainButton"] = [this](){
        pushView(EViewType::Game);
    };
    eventMap["OnBackButton"] = [this](){
        processBackButtonEvent();
    };
    eventMap["OnSurfaceHidden"] = [this](){
        processSurfaceVisible(false);
    };
    eventMap["OnSurfaceShown"] = [this](){
        processSurfaceVisible(true);
    };
    eventMap["Pause_OnRestartButton"] = [this](){
        pushView(EViewType::Game);
    };
    eventMap["Pause_OnOptionsButton"] = [this](){
        pushView(EViewType::Options);
    };
    eventMap["Pause_OnExitToMainButton"] = [this](){
        pushView(EViewType::Main);
    };
    eventMap["Exit_OnYesExit"] = [this](){
        ET_SendEvent(&ETAppRunStateEvents::ET_onTerminate);
    };
}

bool UIEventManager::init() {
    ETNode<ETUIEventManager>::connect(getEntityId());
    ETNode<ETUIViewStackEvents>::connect(getEntityId());
    ETNode<ETUIButtonEventManager>::connect(getEntityId());
    return true;
}

void UIEventManager::deinit() {
    ETNode<ETUIEventManager>::disconnect();
    ETNode<ETUIViewStackEvents>::disconnect();
    ETNode<ETUIButtonEventManager>::disconnect();
}

bool UIEventManager::ET_isSomeButtonPressed() const {
    return isButtonPressed;
}
void UIEventManager::ET_setButtonPressed(bool flag) {
    isButtonPressed = flag;
}

UIEventManager::EViewType UIEventManager::getActiveViewType() const {
    EntityId activeViewId;
    ET_SendEventReturn(activeViewId, &ETUIViewStack::ET_getActiveViewId);
    return getViewTypeFromEntityId(activeViewId);
}

const char* UIEventManager::getViewName(UIEventManager::EViewType viewType) const {
    for(auto& viewNode : viewMap) {
        if(viewNode.second == viewType) {
            return viewNode.first.c_str();
        }
    }
    return nullptr;
}

void UIEventManager::processSurfaceVisible(bool isVisible) {
    if(isVisible) {
        bool isGamePaused = false;
        ET_SendEventReturn(isGamePaused, &ETGameState::ET_isGamePaused);
        if(!isGamePaused) {
            return;
        }
        auto activeViewType = getActiveViewType();
        if(activeViewType == EViewType::Game) {
            auto pauseViewName = getViewName(EViewType::Pause);
            ET_SendEvent(&ETUIViewStack::ET_forcePushView, pauseViewName);
        }
    } else {
        if(getActiveViewType() != EViewType::Game) {
            return;
        }
        ET_SendEvent(&ETGameState::ET_pauseGame);
    }
}

void UIEventManager::processBackButtonEvent() {
    auto activeViewType = getActiveViewType();
    switch (activeViewType)
    {
    case EViewType::EndGame: {
        pushView(EViewType::Main);
        break;
    }
    case EViewType::Main: {
        pushView(EViewType::Exit);
        break;
    }
    case EViewType::Game: {
        pushView(EViewType::Pause);
        break;
    }
    default:
        popView();
    }
}

void UIEventManager::ET_onEvent(const char* eventName) {
    auto it = eventMap.find(eventName);
    if (it == eventMap.end()) {
        LogWarning("[UIEventManager::ET_onEvent] Unknown event name: %s", eventName);
        return;
    }
    auto callbackFunc = it->second;
    callbackFunc();
}

void UIEventManager::pushView(EViewType viewType) {
    auto viewName = getViewName(viewType);
    if(viewType == EViewType::EndGame || viewType == EViewType::Main || viewType == EViewType::Game) {
        ET_SendEvent(&ETUIViewStack::ET_clearAllAndPushNewView, viewName);
    } else {
        ET_SendEvent(&ETUIViewStack::ET_pushView, viewName);
    }
}

void UIEventManager::popView() {
    ET_SendEvent(&ETUIViewStack::ET_popView);
}

UIEventManager::EViewType UIEventManager::getViewTypeFromEntityId(EntityId viewId) const {
    if(!viewId.isValid()) {
        return EViewType::Other;
    }
    std::string viewName;
    ET_SendEventReturn(viewName, viewId, &ETEntity::ET_getName);
    if(viewName.empty()) {
        return EViewType::Other;
    }
    auto it = viewMap.find(viewName);
    if(it != viewMap.end()) {
        return it->second;
    }
    return EViewType::Other;
}

void UIEventManager::ET_onViewStartPush(EntityId viewId) {
    auto viewType = getViewTypeFromEntityId(viewId);
    switch (viewType)
    {
    case EViewType::Pause: {
        ET_SendEvent(&ETGameState::ET_pauseGame);
        break;
    }
    default:
        break;
    }
}

void UIEventManager::ET_onViewFinishPush(EntityId viewId) {
    auto viewType = getViewTypeFromEntityId(viewId);
    switch (viewType)
    {
    case EViewType::Game: {
        ET_SendEvent(&ETGameState::ET_startGame);
        break;
    }
    default:
        break;
    }
}

void UIEventManager::ET_onViewStartPop(EntityId viewId) {
    auto viewType = getViewTypeFromEntityId(viewId);
    switch (viewType)
    {
    case EViewType::Game: {
        ET_SendEvent(&ETGameState::ET_endGame, EEndGameReason::Interrupt);
        break;
    }
    default:
        break;
    }
}

void UIEventManager::ET_onViewFinishPop(EntityId viewId) {
    auto viewType = getViewTypeFromEntityId(viewId);
    switch (viewType)
    {
    case EViewType::Pause: {
        ET_SendEvent(&ETGameState::ET_resumeGame);
        break;
    }
    default:
        break;
    }
}