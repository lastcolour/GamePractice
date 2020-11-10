#include "Game/States/PostGameState.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGame.hpp"
#include "Game/ETGameElem.hpp"

PostGameState::PostGameState() :
    postGameTime(1.f) {
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
    postGameTime = 1.f;
    ETNode<ETGameTimerEvents>::connect(gameEntityId);
}

void PostGameState::onLeave() {
    ET_SendEvent(&ETGameStateEvents::ET_onGameLeaveState, EGameState::PostGame);

    ETNode<ETGameTimerEvents>::disconnect();
    setupEndResult();
    gameEntityId = InvalidEntityId;
}

void PostGameState::ET_onGameTick(float dt) {
    postGameTime -= dt;
    if(postGameTime < 0.f) {
        ET_SendEvent(&ETGameStateManager::ET_changeState, EGameState::None);
    }
}

void PostGameState::setupEndResult() {
    ObjectiveProgress objectiveProgress = ObjectiveProgress::Fail;
    ET_SendEventReturn(endResult, &ETGameScore::ET_getGameResult);

    int highScore = 0;
    ET_SendEventReturn(highScore, &ETGameConfig::ET_getHighScore);
    bool newHighScore = false;
    if(endResult.score > highScore) {
        ET_SendEvent(&ETGameConfig::ET_setHighScore, endResult.score);
        endResult.newHighScore = true;
    }
}

const EndGameResult& PostGameState::ET_getLastGameResult() const {
    return endResult;
}