#include "Game/States/PostGameState.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGame.hpp"

PostGameState::PostGameState() :
    postGameTime(1.f) {
}

PostGameState::~PostGameState() {
}

void PostGameState::onEnter(EntityId gameId) {
    gameEntityId = gameId;
    ETNode<ETGameEndResult>::connect(gameId);
    bool isAllElemStatic = false;
    ET_SendEventReturn(isAllElemStatic, &ETGameBoard::ET_isAllElemStatic);
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
    int gameScore = 0;
    ET_SendEventReturn(gameScore, &ETGameScore::ET_getGameScore);
    int highScore = 0;
    ET_SendEventReturn(highScore, &ETGameConfig::ET_getHighScore);
    bool newHighScore = false;
    if(gameScore > highScore) {
        ET_SendEvent(&ETGameConfig::ET_setHighScore, gameScore);
        newHighScore = true;
    }
    endResult.reset(new EndGameResult);
    endResult->score = gameScore;
    endResult->newHighScore = newHighScore;
}

const EndGameResult* PostGameState::ET_getGameEndResult() const {
    if(endResult) {
        return endResult.get();
    }
    return nullptr;
}