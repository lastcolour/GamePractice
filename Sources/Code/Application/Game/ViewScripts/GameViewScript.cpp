#include "Game/ViewScripts/GameViewScript.hpp"
#include "Game/ETGame.hpp"
#include "Game/ETGameTimer.hpp"
#include "UI/ETUIView.hpp"
#include "UI/ETUIBox.hpp"
#include "Game/ETGameBoardSpawner.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/ETGameMusic.hpp"

#include <cassert>

void GameViewScript::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameViewScript>("GameViewScript")) {
        classInfo->addField("stars", &GameViewScript::progressStars);
        classInfo->addField("timeInfoBox", &GameViewScript::timeInfoBoxId);
        classInfo->addField("timeValueLabelId", &GameViewScript::timeValueLabelId);
        classInfo->addField("movesInfoBoxId", &GameViewScript::movesInfoBoxId);
        classInfo->addField("movesValueLabelId", &GameViewScript::movesValueLabelId);
        classInfo->addField("scoreInfoBox", &GameViewScript::scoreInfoBoxId);
        classInfo->addField("startInfo", &GameViewScript::startInfoId);
        classInfo->addField("endInfo", &GameViewScript::endInfoId);
        classInfo->addField("postGameTime", &GameViewScript::postGameTime);
        classInfo->addField("getStartSound", &GameViewScript::getStarEvent);
    }
}

GameViewScript::GameViewScript() :
    postGameTime(1.f),
    scriptState(State::None) {
}

GameViewScript::~GameViewScript() {
}

void GameViewScript::init() {
    BaseViewScript::init();

    ETNode<ETGameObjectiveEvents>::connect(getEntityId());
    ETNode<ETGameStateEvents>::connect(getEntityId());
    ETNode<ETSurfaceEvents>::connect(getEntityId());
    ETNode<ETSurfaceEvents>::connect(getEntityId());

    eventSeq.init(getEntityId());
}

void GameViewScript::ET_onViewOpened() {
    BaseViewScript::ET_onViewOpened();

    ET_SendEvent(scoreInfoBoxId, &ETUIElement::ET_show);
    ET_SendEvent(progressStars.fristId, &ETUIElement::ET_hide);
    ET_SendEvent(progressStars.secondId, &ETUIElement::ET_hide);
    ET_SendEvent(progressStars.thirdId, &ETUIElement::ET_hide);
    ET_SendEvent(startInfoId, &ETUIElement::ET_hide);
    ET_SendEvent(endInfoId, &ETUIElement::ET_hide);

    ET_SendEvent(&ETGameScoreUpdater::ET_reset);
    ET_SendEvent(&ETGameBoardSpawner::ET_loadPendingLevel);

    EGameLimitType gameLimitType = EGameLimitType::None;
    ET_SendEventReturn(gameLimitType, &ETGameLimits::ET_getLimitType);
    switch(gameLimitType) {
        case EGameLimitType::Moves: {
            ET_SendEvent(movesInfoBoxId, &ETUIElement::ET_show);
            ET_SendEvent(timeInfoBoxId, &ETUIElement::ET_hide);
            ET_SendEvent(&ETGameLimits::ET_setLabelId, movesValueLabelId);
            break;
        }
        case EGameLimitType::Time: {
            ET_SendEvent(movesInfoBoxId, &ETUIElement::ET_hide);
            ET_SendEvent(timeInfoBoxId, &ETUIElement::ET_show);
            ET_SendEvent(&ETGameLimits::ET_setLabelId, timeValueLabelId);
            break;
        }
        case EGameLimitType::None: {
            LogWarning("[GameViewScript::ET_onViewOpened] Level doesn't have any limit logic");
            ET_SendEvent(movesInfoBoxId, &ETUIElement::ET_hide);
            ET_SendEvent(timeInfoBoxId, &ETUIElement::ET_hide);
            break;
        }
        default: {
            assert(false && "Invalid limit type");
        }
    }

    scriptState = State::None;
}

void GameViewScript::ET_onViewClosed() {
    BaseViewScript::ET_onViewClosed();
    scriptState = State::None;
    ET_SendEvent(&ETGameBoardSpawner::ET_unloadLevel);
    ET_SendEvent(&ETGameStateManager::ET_finishGame);
}

void GameViewScript::ET_onViewLostFocus() {
    BaseViewScript::ET_onViewLostFocus();
    ET_SendEvent(&ETGameStateManager::ET_pauseGame);
}

void GameViewScript::ET_onViewGetFocus() {
    BaseViewScript::ET_onViewGetFocus();

    ET_SendEvent(&ETGameMusic::ET_setTheme, EMusicTheme::Game);

    switch(scriptState) {
        case State::None: {
            setScriptState(State::ShowingStartInfo);
            EventSequence::Event event;
            event.targetId = startInfoId;
            event.animType = EAnimSequenceType::Appear;
            event.onEndCallback = [this](){
                setScriptState(State::Game);
                ET_SendEvent(&ETGameStateManager::ET_startGame);
            };
            eventSeq.addEvent(event);
            eventSeq.start();
            break;
        }
        case State::ShowingStartInfo: {
            break;
        }
        case State::Game: {
            ET_SendEvent(&ETGameStateManager::ET_resumeGame);
            break;
        }
        case State::ShowingEndInfo: {
            break;
        }
        case State::WaitingPostGame: {
            break;
        }
        case State::WaitingEnd: {
            break;
        }
        default: {
            assert(false && "invalid script state");
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
        case State::ShowingStartInfo: {
            eventSeq.forceFinish();
            break;
        }
        case State::Game: {
            ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::PauseGame);
            break;
        }
        case State::ShowingEndInfo: {
            eventSeq.forceFinish();
        }
        case State::WaitingPostGame: {
            break;
        }
        case State::WaitingEnd: {
            break;
        }
        default: {
            assert(false && "invalid script state");
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
        case ObjectiveProgress::Fail: {
            break;
        }
        default: {
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
    if(state != EGameState::PostGame) {
        return;
    }
    setScriptState(State::ShowingEndInfo);

    if(!UI::PlayAnimation(timeInfoBoxId, EAnimSequenceType::Disappear, InvalidEntityId)) {
        ET_SendEvent(timeInfoBoxId, &ETUIElement::ET_hide);
    }
    if(!UI::PlayAnimation(movesInfoBoxId, EAnimSequenceType::Disappear, InvalidEntityId)) {
        ET_SendEvent(movesInfoBoxId, &ETUIElement::ET_hide);
    }
    ET_SendEvent(&ETGameTimer::ET_setScale, 0.1f);
    ET_SendEvent(&ETGameScoreUpdater::ET_pause);

    {
        EventSequence::Event event;
        event.animType = EAnimSequenceType::Appear;
        event.targetId = endInfoId;
        event.onEndCallback = [this](){
            ET_SendEvent(&ETGameScoreUpdater::ET_resume);
            ET_SendEvent(&ETGameTimer::ET_setScale, 1.f);
            setScriptState(State::WaitingPostGame);
        };
        eventSeq.addEvent(event);
    }

    eventSeq.start();
}

void GameViewScript::ET_onGameLeaveState(EGameState gameState) {
    if(gameState != EGameState::PostGame) {
        return;
    }
    {
        EventSequence::Event event;
        event.onEndCallback = [this](){
            setScriptState(State::WaitingEnd);
        };
        eventSeq.addEvent(event);
    }
    {
        EventSequence::Event event;
        event.startDelay = postGameTime;
        event.onEndCallback = [this](){
            setScriptState(State::None);
            ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Game);
            ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::EndGame);
        };
        eventSeq.addEvent(event);
    }
    eventSeq.start();
}

void GameViewScript::ET_onSurfaceHidden() {
    if(!hasFocus()) {
        return;
    }
    switch(scriptState) {
        case State::None: {
            break;
        }
        case State::ShowingStartInfo: {
            eventSeq.forceFinish();
            ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::PauseGame);
            break;
        }
        case State::Game: {
            ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::PauseGame);
            break;
        }
        case State::ShowingEndInfo: {
            break;
        }
        case State::WaitingPostGame: {
            break;
        }
        case State::WaitingEnd: {
            eventSeq.forceFinish();
            break;
        }
        default: {
            assert(false && "invalid script state");
        }
    }
}

void GameViewScript::setScriptState(State newState) {
    switch(newState) {
        case State::None: {
            LogDebug("[GameViewScript::setScriptState] Set new state: 'None'");
            break;
        }
        case State::ShowingStartInfo: {
            assert(scriptState == State::None && "invalid prev state");
            LogDebug("[GameViewScript::setScriptState] Set new state: 'ShowingStartInfo'");
            break;
        }
        case State::Game: {
            assert(scriptState == State::ShowingStartInfo && "invalid prev state");
            LogDebug("[GameViewScript::setScriptState] Set new state: 'Game'");
            break;
        }
        case State::ShowingEndInfo: {
            assert(scriptState == State::Game && "invalid prev state");
            LogDebug("[GameViewScript::setScriptState] Set new state: 'ShowingEndInfo'");
            break;
        }
        case State::WaitingPostGame: {
            assert(scriptState == State::ShowingEndInfo && "invalid prev state");
            LogDebug("[GameViewScript::setScriptState] Set new state: 'WaitingPostGame'");
            break;
        };
        case State::WaitingEnd: {
            assert(scriptState == State::WaitingPostGame && "invalid prev state");
            LogDebug("[GameViewScript::setScriptState] Set new state: 'WaitingEnd'");
            break;
        }
        default: {
            assert(false && "invalid script state");
            break;
        }
    }
    scriptState = newState;
}