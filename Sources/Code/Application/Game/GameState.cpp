#include "Game/GameState.hpp"

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
}

EGameState GameState::ET_getGameState() const {
    return gameState;
}