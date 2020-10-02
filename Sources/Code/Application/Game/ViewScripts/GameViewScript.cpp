#include "Game/ViewScripts/GameViewScript.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Game/ETGame.hpp"
#include "Game/ETGameTimer.hpp"
#include "UI/ETUIView.hpp"

void GameViewScript::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameViewScript>("GameViewScript")) {
    }
}

GameViewScript::GameViewScript() {
}

GameViewScript::~GameViewScript() {
}

void GameViewScript::ET_onLostFocus() {
    BaseViewScript::ET_onLostFocus();
    ET_SendEvent(&ETGameStateManager::ET_pauseGame);
}

void GameViewScript::ET_onGetFocus() {
    BaseViewScript::ET_onGetFocus();
    bool isGameStarted = false;
    ET_SendEventReturn(isGameStarted, &ETGameStateManager::ET_isInGameState);
    if(!isGameStarted) {
        ET_SendEvent(&ETGameStateManager::ET_startGame);
    } else {
        ET_SendEvent(&ETGameStateManager::ET_resumeGame);
    }
}

void GameViewScript::onEvent(const UIEvent& event) {
    if(event.type == UIEvent::EventType::OnBackButton) {
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::PauseGame);
    } else if(event.type == UIEvent::EventType::OnGameGameEnd) {
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::EndGame);
    }
}
