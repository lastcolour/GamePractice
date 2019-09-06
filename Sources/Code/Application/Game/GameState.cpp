#include "Game/GameState.hpp"

GameState::GameState() {
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
    ET_SendEvent(&ETGameTimer::ET_resumeTimer);
}

void GameState::ET_pauseGame() {
    ET_SendEvent(&ETGameTimer::ET_pauseTimer);
}

void GameState::ET_resumeGame() {
    ET_SendEvent(&ETGameTimer::ET_resumeTimer);
}

void GameState::ET_endGame() {
}