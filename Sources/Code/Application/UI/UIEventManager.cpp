#include "UI/UIEventManager.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"

enum class EUIEventType {
    Main_OnStartButton,
    Main_onPlayerButton,
    Main_OnAboutButton,
    Main_OnOptionsButton,
    Player_OnBackButton,
    About_OnBackButton,
    Options_OnBackButton,
    Game_OnBackButton,
    Game_OnGameEnd,
    EndGame_OnExitToMainButton,
    EndGame_OnPlayAgainButton
};

UIEventManager::UIEventManager() {
    eventMap["Main_OnStartButton"] = EUIEventType::Main_OnStartButton;
    eventMap["Main_OnPlayerButton"] = EUIEventType::Main_onPlayerButton;
    eventMap["Main_OnAboutButton"] = EUIEventType::Main_OnAboutButton;
    eventMap["Main_OnOptionsButton"] = EUIEventType::Main_OnOptionsButton;
    eventMap["Player_OnBackButton"] = EUIEventType::Player_OnBackButton;
    eventMap["About_OnBackButton"] = EUIEventType::About_OnBackButton;
    eventMap["Options_OnBackButton"] = EUIEventType::Options_OnBackButton;
    eventMap["Game_OnBackButton"] = EUIEventType::Game_OnBackButton;
    eventMap["Game_OnGameEnd"] = EUIEventType::Game_OnGameEnd;
    eventMap["EndGame_OnExitToMainButton"] = EUIEventType::EndGame_OnExitToMainButton;
    eventMap["EndGame_OnPlayAgainButton"] = EUIEventType::EndGame_OnPlayAgainButton;
}

UIEventManager::~UIEventManager() {
}

bool UIEventManager::init() {
    ETNode<ETUIEventManager>::connect(getEntityId());
    return true;
}

void UIEventManager::deinit() {
}

void UIEventManager::processEvent(EUIEventType eventType) {
    switch (eventType)
    {
        case EUIEventType::Main_OnStartButton: {
            ET_SendEvent(&ETUIViewManager::ET_openView, "UI/GameView/Root.json");
            break;
        }
        case EUIEventType::Main_onPlayerButton: {
            ET_SendEvent(&ETUIViewManager::ET_openView, "UI/PlayerView/Root.json");
            break;
        }
        case EUIEventType::Main_OnAboutButton: {
            ET_SendEvent(&ETUIViewManager::ET_openView, "UI/AboutView/Root.json");
            break;
        }
        case EUIEventType::Main_OnOptionsButton: {
            ET_SendEvent(&ETUIViewManager::ET_openView, "UI/OptionsView/Root.json");
            break;
        }
        case EUIEventType::Player_OnBackButton: {
            ET_SendEvent(&ETUIViewManager::ET_openView, "UI/MainView/Root.json");
            break;
        }
        case EUIEventType::About_OnBackButton: {
            ET_SendEvent(&ETUIViewManager::ET_openView, "UI/MainView/Root.json");
            break;
        }
        case EUIEventType::Options_OnBackButton: {
            ET_SendEvent(&ETUIViewManager::ET_openView, "UI/MainView/Root.json");
            break;
        }
        case EUIEventType::Game_OnBackButton: {
            ET_SendEvent(&ETUIViewManager::ET_openView, "UI/MainView/Root.json");
            break;
        }
        case EUIEventType::Game_OnGameEnd: {
            ET_SendEvent(&ETUIViewManager::ET_openView, "UI/EndGameView/Root.json");
            break;
        }
        case EUIEventType::EndGame_OnExitToMainButton: {
            ET_SendEvent(&ETUIViewManager::ET_openView, "UI/MainView/Root.json");
            break;
        }
        case EUIEventType::EndGame_OnPlayAgainButton: {
            ET_SendEvent(&ETUIViewManager::ET_openView, "UI/GameView/Root.json");
            break;
        }
        default:
            break;
    }
}

void UIEventManager::ET_onEvent(const char* eventName) {
    auto it = eventMap.find(eventName);
    if (it == eventMap.end()) {
        LogWarning("[UIEventManager::ET_onEvent] Unknown event name: %s", eventName);
        return;
    }
    processEvent(it->second);
}