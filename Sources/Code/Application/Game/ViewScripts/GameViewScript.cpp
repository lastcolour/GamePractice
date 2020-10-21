#include "Game/ViewScripts/GameViewScript.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Game/ETGame.hpp"
#include "Game/ETGameTimer.hpp"
#include "UI/ETUIView.hpp"
#include "UI/ETUIAnimation.hpp"
#include "Render/ETRenderNode.hpp"
#include "Audio/ETSound.hpp"

void GameViewScript::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameViewScript>("GameViewScript")) {
        classInfo->addField("stars", &GameViewScript::progressStars);
        classInfo->addField("timeInfoBox", &GameViewScript::timeInfoBoxId);
        classInfo->addField("boardSpawner", &GameViewScript::boardSpawnerId);
        classInfo->addResourceField("getStartSound", ResourceType::SoundEvent, &GameViewScript::setGetStatSoundEvent);
    }
}

GameViewScript::GameViewScript() {
}

GameViewScript::~GameViewScript() {
}

bool GameViewScript::init() {
    BaseViewScript::init();
    ETNode<ETGameObjectiveEvents>::connect(getEntityId());
    ETNode<ETGameStateEvents>::connect(getEntityId());
    return true;
}

void GameViewScript::setGetStatSoundEvent(const char* eventName) {
    ET_SendEventReturn(getStarEvent, &ETSoundManager::ET_createEvent, eventName);
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

void GameViewScript::ET_onObjectiveCompleted(ObjectiveProgress type) {
    EntityId starId;
    switch(type) {
        case ObjectiveProgress::OneStar: {
            starId = progressStars.fristId;
            break;
        }
        case ObjectiveProgress::TwoStars: {
            starId = progressStars.secondId;
            break;
        }
        case ObjectiveProgress::ThreeStars: {
            starId = progressStars.thirdId;
            break;
        }
    }
    if(!starId.isValid()) {
        return;
    }
    getStarEvent.emit();
    ET_SendEvent(starId, &ETRenderNode::ET_show);
    ET_SendEvent(starId, &ETUIViewAppearAnimation::ET_appear);
}

void GameViewScript::ET_onGameEnterState(EGameState state) {
    if(state == EGameState::PostGame) {
        ET_SendEvent(timeInfoBoxId, &ETUIViewAppearAnimation::ET_disappear);
    }
}

void GameViewScript::ET_onGameLeaveState(EGameState state) {
}