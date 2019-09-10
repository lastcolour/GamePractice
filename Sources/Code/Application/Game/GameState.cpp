#include "Game/GameState.hpp"
#include "UI/ETUIInterfaces.hpp"

namespace {

const char* END_EVENT_NORMAL = "Game_OnGameNormalEnd";
const char* END_EVENT_NEW_HIGH_SCORE = "Game_OnGameHighScoreEnd";

}; // namespace

GameState::GameState() :
    gameState(EGameState::None) {
}

GameState::~GameState() {
}

bool GameState::init() {
    ETNode<ETGameState>::connect(getEntityId());
    ET_SendEvent(&ETGameTimer::ET_pauseTimer);
    return true;
}

void GameState::deinit() {
    ETNode<ETGameState>::disconnect();
}

void GameState::ET_startGame() {
    gameState = EGameState::Running;
    ET_SendEvent(&ETGameTimer::ET_resumeTimer);
}

void GameState::ET_pauseGame() {
    gameState = EGameState::Paused;
    ET_SendEvent(&ETGameTimer::ET_pauseTimer);
}

void GameState::ET_resumeGame() {
    gameState = EGameState::Running;
    ET_SendEvent(&ETGameTimer::ET_resumeTimer);
}

void GameState::ET_endGame() {
    gameState = EGameState::None;

    int prevHighScore = 0;
    ET_SendEventReturn(prevHighScore, &ETGameConfig::ET_getHighScore);

    int currScore = 0;
    ET_SendEventReturn(currScore, &ETGameScore::ET_getGameScore);

    if(currScore > prevHighScore) {
        ET_SendEvent(&ETGameConfig::ET_setHighScore, currScore);
        ET_SendEvent(&ETUIEventManager::ET_onEvent, END_EVENT_NEW_HIGH_SCORE);
    } else {
        ET_SendEvent(&ETUIEventManager::ET_onEvent, END_EVENT_NORMAL);
    }
}

void GameState::ET_interruptGame() {
    gameState = EGameState::None;
}

EGameState GameState::ET_getGameState() const {
    return gameState;
}