#include "Game/States/PostGameState.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGame.hpp"
#include "Game/Progression/LevelsProgressData.hpp"
#include "Game/ETGameBoardSpawner.hpp"
#include "Game/ETLevelProgress.hpp"
#include "Game/GameUtils.hpp"

PostGameState::PostGameState() :
    allElemStatic(false),
    zoomOutPlayed(false) {
}

PostGameState::~PostGameState() {
}

void PostGameState::onEnter(EntityId gameId) {
    allElemStatic = false;
    zoomOutPlayed = false;

    ET_SendEvent(&ETGameStateEvents::ET_onGameEnterState, EGameState::PostGame);

    gameEntityId = gameId;
    ETNode<ETGameEndResult>::connect(gameId);

    allElemStatic = true;
    ET_SendEventReturn(allElemStatic, &ETGameBoard::ET_isAllElemStatic);
    if(!allElemStatic) {
        ETNode<ETGameBoardEvents>::connect(gameId);
    } else {
        ET_onAllElemsStatic();
    }

    if(ET_IsExistNode<ETGameBoardAnimation>()) {
        ETNode<ETGameBoardAnimationEvents>::connect(gameId);
        ET_SendEvent(&ETGameBoardAnimation::ET_zoomOut);
    } else {
        zoomOutPlayed = true;
        ET_onZoomOutPlayed();
    }
}

void PostGameState::ET_onAllElemsStatic() {
    allElemStatic = true;
    ETNode<ETGameBoardEvents>::disconnect();
    tryEndPostGame();
}

void PostGameState::ET_onZoomOutPlayed() {
    zoomOutPlayed = true;
    ETNode<ETGameBoardAnimationEvents>::disconnect();
    tryEndPostGame();
}

void PostGameState::onLeave() {
    ETNode<ETGameBoardEvents>::disconnect();
    setupEndResult();
    gameEntityId = InvalidEntityId;
    ET_SendEvent(&ETGameStateEvents::ET_onGameLeaveState, EGameState::PostGame);
}

void PostGameState::setupEndResult() {
    ET_SendEventReturn(endResult, &ETGameScore::ET_getGameResult);

    LevelProgress newProgress;
    ET_SendEventReturn(newProgress.name, &ETGameBoardSpawner::ET_getLevelName);
    newProgress.completeTime = endResult.gameTime;
    newProgress.score = endResult.score;
    newProgress.stars = GameUtils::ConvertToStarsCount(endResult.objectiveCompleted);

    ET_SendEvent(&ETLevelsProgression::ET_setLevelProgress, newProgress);
}

const EndGameResult& PostGameState::ET_getLastGameResult() const {
    return endResult;
}

void PostGameState::tryEndPostGame() {
    if(allElemStatic && zoomOutPlayed) {
        ET_SendEvent(&ETGameStateManager::ET_changeState, EGameState::None);
    }
}