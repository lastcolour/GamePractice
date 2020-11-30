#include "Game/States/PostGameState.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGame.hpp"
#include "Game/ETGameElem.hpp"
#include "Game/Progression/LevelsProgressData.hpp"
#include "Game/ETGameBoardSpawner.hpp"
#include "Game/ETLevelProgress.hpp"
#include "Game/GameUtils.hpp"

PostGameState::PostGameState() {
}

PostGameState::~PostGameState() {
}

void PostGameState::onEnter(EntityId gameId) {
    ET_SendEvent(&ETGameStateEvents::ET_onGameEnterState, EGameState::PostGame);

    gameEntityId = gameId;
    ETNode<ETGameEndResult>::connect(gameId);
    bool isAllElemStatic = false;
    ET_SendEventReturn(isAllElemStatic, &ETGameBoard::ET_isAllElemStatic);
    ET_SendEvent(&ETGameBoardAnimation::ET_zoomOut);
    if(!isAllElemStatic) {
        ETNode<ETGameBoardEvents>::connect(gameId);
    } else {
        ET_onAllElemsStatic();
    }
}

void PostGameState::ET_onAllElemsStatic() {
    ET_SendEvent(&ETGameStateManager::ET_changeState, EGameState::None);
}

void PostGameState::onLeave() {
    ETNode<ETGameBoardEvents>::disconnect();
    setupEndResult();
    gameEntityId = InvalidEntityId;
    ET_SendEvent(&ETGameStateEvents::ET_onGameLeaveState, EGameState::PostGame);
}

void PostGameState::setupEndResult() {
    ObjectiveProgress objectiveProgress = ObjectiveProgress::Fail;
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