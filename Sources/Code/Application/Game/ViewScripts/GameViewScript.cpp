#include "Game/ViewScripts/GameViewScript.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Game/ETGame.hpp"
#include "Game/ETGameTimer.hpp"
#include "UI/ETUIView.hpp"
#include "Audio/ETSound.hpp"
#include "UI/ETUIBox.hpp"
#include "Game/ETGameBoardSpawner.hpp"
#include "Game/ETGameInterfaces.hpp"

#include <cassert>

void GameViewScript::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameViewScript>("GameViewScript")) {
        classInfo->addField("stars", &GameViewScript::progressStars);
        classInfo->addField("timeInfoBox", &GameViewScript::timeInfoBoxId);
        classInfo->addField("boardSpawner", &GameViewScript::boardSpawnerId);
        classInfo->addField("startInfo", &GameViewScript::startInfoId);
        classInfo->addField("endInfo", &GameViewScript::endInfoId);
        classInfo->addField("postGameTime", &GameViewScript::postGameTime);
        classInfo->addResourceField("getStartSound", ResourceType::SoundEvent, &GameViewScript::setGetStatSoundEvent);
    }
}

GameViewScript::GameViewScript() :
    postGameTime(1.f),
    currentPostGameTime(0.f),
    scriptState(State::None) {
}

GameViewScript::~GameViewScript() {
}

bool GameViewScript::init() {
    BaseViewScript::init();
    ETNode<ETGameObjectiveEvents>::connect(getEntityId());
    ETNode<ETGameStateEvents>::connect(getEntityId());
    ETNode<ETUIAnimationSequenceEvent>::connect(getEntityId());
    return true;
}

void GameViewScript::setGetStatSoundEvent(const char* eventName) {
    ET_SendEventReturn(getStarEvent, &ETSoundManager::ET_createEvent, eventName);
}

void GameViewScript::ET_onViewOpened() {
    BaseViewScript::ET_onViewOpened();

    ET_SendEvent(timeInfoBoxId, &ETUIElement::ET_show);
    ET_SendEvent(progressStars.fristId, &ETUIElement::ET_hide);
    ET_SendEvent(progressStars.secondId, &ETUIElement::ET_hide);
    ET_SendEvent(progressStars.thirdId, &ETUIElement::ET_hide);
    ET_SendEvent(startInfoId, &ETUIElement::ET_hide);
    ET_SendEvent(endInfoId, &ETUIElement::ET_hide);

    ET_SendEvent(&ETGameScoreUpdater::ET_reset);
    ET_SendEvent(&ETGameBoardSpawner::ET_loadPendingLevel);

    scriptState = State::None;
}

void GameViewScript::ET_onViewClosed() {
    scriptState = State::None;
    BaseViewScript::ET_onViewClosed();
    ET_SendEvent(&ETGameBoardSpawner::ET_unloadLevel);
    ET_SendEvent(&ETGameStateManager::ET_finishGame);
}

void GameViewScript::ET_onViewLostFocus() {
    BaseViewScript::ET_onViewLostFocus();
    ET_SendEvent(&ETGameStateManager::ET_pauseGame);
}

void GameViewScript::ET_onViewGetFocus() {
    BaseViewScript::ET_onViewGetFocus();
    switch(scriptState) {
        case State::None: {
            scriptState = State::ShowStartInfo;
            if(!UI::PlayAnimation(startInfoId, EAnimSequenceType::Appear, getEntityId())) {
                ET_onAnimationPlayed(startInfoId, EAnimSequenceType::Appear);
            }
            break;
        }
        case State::ShowStartInfo: {
            break;
        }
        case State::Game: {
            ET_SendEvent(&ETGameStateManager::ET_resumeGame);
            break;
        }
        case State::ShowEndInfo: {
            break;
        }
        case State::WaitPostGame: {
            break;
        }
        default: {
            assert(false && "Invalid script state");
        }
    }
}

void GameViewScript::onEvent(const UIEvent& event) {
    if(event.type != UIEvent::EventType::OnBackButton) {
        return;
    }
    switch(scriptState) {
        case State::None: {
            break;
        }
        case State::ShowStartInfo: {
            ET_SendEvent(startInfoId, &ETUITimerEvents::ET_onUITick, 64.f);
            break;
        }
        case State::Game: {
            ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::PauseGame);
            break;
        }
        case State::ShowEndInfo: {
            ET_SendEvent(endInfoId, &ETUITimerEvents::ET_onUITick, 64.f);
            break;
        }
        case State::WaitPostGame: {
            ET_onUITick(64.f);
            break;
        }
        default: {
            assert(false && "Invalid script state");
        }
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
    if(!UI::PlayAnimation(starId, EAnimSequenceType::Appear, InvalidEntityId)) {
        ET_SendEvent(starId, &ETUIElement::ET_show);
    }
}

void GameViewScript::ET_onGameEnterState(EGameState state) {
    if(state == EGameState::PostGame) {
        assert(scriptState == State::Game && "Invalid script state");
        UI::PlayAnimation(timeInfoBoxId, EAnimSequenceType::Disappear, InvalidEntityId);
        scriptState = State::ShowEndInfo;
        ET_SendEvent(&ETGameTimer::ET_pause);
        if(!UI::PlayAnimation(endInfoId, EAnimSequenceType::Appear, getEntityId())) {
            ET_onAnimationPlayed(endInfoId, EAnimSequenceType::Appear);
        }
    }
}

void GameViewScript::ET_onUITick(float dt) {
    currentPostGameTime -= dt;
    if(currentPostGameTime < 0.f) {
        assert(scriptState == State::WaitPostGame && "Invalid script state");
        scriptState = State::None;
        ETNode<ETUITimerEvents>::disconnect();
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::EndGame);
    }
}

void GameViewScript::ET_onGameLeaveState(EGameState gameState) {
    if(gameState == EGameState::PostGame) {
        assert(scriptState == State::WaitPostGame && "Invalid script state");
        currentPostGameTime = postGameTime;
        ETNode<ETUITimerEvents>::connect(getEntityId());
    }
}

void GameViewScript::ET_onAnimationPlayed(EntityId sourceId, EAnimSequenceType animType) {
    switch(scriptState) {
        case State::None: {
            break;
        }
        case State::ShowStartInfo: {
            scriptState = State::Game;
            ET_SendEvent(&ETGameStateManager::ET_startGame);
            break;
        }
        case State::Game: {
            break;
        }
        case State::ShowEndInfo: {
            scriptState = State::WaitPostGame;
            ET_SendEvent(&ETGameTimer::ET_resume);
            break;
        }
        case State::WaitPostGame: {
            break;
        }
        default: {
            assert(false && "Invalid script state");
        }
    }
}