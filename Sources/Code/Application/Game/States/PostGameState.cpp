#include "Game/States/PostGameState.hpp"
#include "Game/ETGameInterfaces.hpp"

PostGameState::PostGameState() :
    postGameTime(1.f) {
}

PostGameState::~PostGameState() {
}

void PostGameState::onEnter(EntityId gameEntityId) {
    ETNode<ETGameEndResult>::connect(gameEntityId);
    ETNode<ETGameTimerEvents>::connect(gameEntityId);
    postGameTime = 1.f;
}

void PostGameState::onLeave() {
    ETNode<ETGameTimerEvents>::disconnect();
    setupEndResult();
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