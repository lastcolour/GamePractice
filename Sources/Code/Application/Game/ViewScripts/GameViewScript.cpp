#include "Game/ViewScripts/GameViewScript.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Game/ETGame.hpp"
#include "Game/ETGameTimer.hpp"
#include "UI/ETUIView.hpp"
#include "UI/ETUIAnimation.hpp"
#include "Audio/ETSound.hpp"
#include "UI/ETUIBox.hpp"
#include "Game/ETGameBoardSpawner.hpp"
#include "Game/ETGameInterfaces.hpp"

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

void GameViewScript::ET_onViewOpened() {
    BaseViewScript::ET_onViewOpened();
    ET_SendEvent(timeInfoBoxId, &ETUIElement::ET_show);
    ET_SendEvent(&ETGameScoreUpdater::ET_reset);
    ET_SendEvent(progressStars.fristId, &ETUIElement::ET_hide);
    ET_SendEvent(progressStars.secondId, &ETUIElement::ET_hide);
    ET_SendEvent(progressStars.thirdId, &ETUIElement::ET_hide);
    ET_SendEvent(&ETGameBoardSpawner::ET_loadPendingLevel);
}

void GameViewScript::ET_onViewClosed() {
    BaseViewScript::ET_onViewClosed();
    ET_SendEvent(&ETGameBoardSpawner::ET_unloadLevel);
}

void GameViewScript::ET_onViewLostFocus() {
    BaseViewScript::ET_onViewLostFocus();
    ET_SendEvent(&ETGameStateManager::ET_pauseGame);
}

void GameViewScript::ET_onViewGetFocus() {
    BaseViewScript::ET_onViewGetFocus();
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
    ET_SendEvent(starId, &ETUIViewAppearAnimation::ET_appear, getEntityId());
}

void GameViewScript::ET_onGameEnterState(EGameState state) {
    if(state == EGameState::PostGame) {
        ET_SendEvent(timeInfoBoxId, &ETUIViewAppearAnimation::ET_disappear, getEntityId());
    }
}

void GameViewScript::ET_onGameLeaveState(EGameState state) {
    if(state == EGameState::PostGame) {
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::EndGame);
    }
}