#include "Game/States/PostGameState.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"

namespace {

const char* GAME_END_EVENT = "Game_OnGameEnd";

} // namespace

PostGameState::PostGameState() {
}

PostGameState::~PostGameState() {
}

void PostGameState::connect(EntityId entityId) {
    ETNode<ETGameEndResult>::connect(entityId);
}

void PostGameState::onEnter() {
}

void PostGameState::onLeave() {
    setupEndResult();
    ET_SendEvent(&ETUIEventManager::ET_onEvent, GAME_END_EVENT);
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