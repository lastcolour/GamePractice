#include "UI/UIEventManager.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Entity/ETEntityInterfaces.hpp"

UIEventManager::UIEventManager() {
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
    return true;
}

void UIEventManager::deinit() {
    ETNode<ETUIEventManager>::disconnect();
}

void UIEventManager::pushView(EViewType viewType) {
    auto viewName = getViewName(viewType);
    if(viewType == EViewType::EndGame || viewType == EViewType::Main || viewType == EViewType::Game) {
        ET_SendEvent(&ETUIViewStack::ET_clearAllAndPushNewView, viewName);
    } else {
        ET_SendEvent(&ETUIViewStack::ET_pushView, viewName);
    }
}

UIEventManager::EViewType UIEventManager::getActiveViewType() const {
    EntityId activeViewId;
    ET_SendEventReturn(activeViewId, &ETUIViewStack::ET_getActiveViewId);
    if(!activeViewId.isValid()) {
        return EViewType::Other;
    }
    std::string viewName;
    ET_SendEventReturn(viewName, activeViewId, &ETEntity::ET_getName);
    if(viewName.empty()) {
        return EViewType::Other;
    }
    auto it = viewMap.find(viewName);
    if(it != viewMap.end()) {
        return it->second;
    }
    return EViewType::Other;
}

const char* UIEventManager::getViewName(UIEventManager::EViewType viewType) const {
    for(auto& viewNode : viewMap) {
        if(viewNode.second == viewType) {
            return viewNode.first.c_str();
        }
    }
    return nullptr;
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
        ET_SendEvent(&ETUIViewStack::ET_popView);
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