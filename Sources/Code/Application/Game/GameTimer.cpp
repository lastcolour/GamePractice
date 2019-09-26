#include "Game/GameTimer.hpp"

GameTimer::GameTimer() :
    isPaused(true) {
}

GameTimer::~GameTimer() {
}

bool GameTimer::init() {
    ETNode<ETGameTimer>::connect(getEntityId());
    ETNode<ETTimerEvents>::connect(getEntityId());
    return true;
}

void GameTimer::deinit() {
    ETNode<ETGameTimer>::disconnect();
    ETNode<ETTimerEvents>::disconnect();
}

void GameTimer::ET_pauseTimer() {
    isPaused = true;
}

void GameTimer::ET_resumeTimer() {
    isPaused = false;
}

void GameTimer::ET_onTick(float dt) {
    if(isPaused) {
        return;
    }
    ET_SendEvent(&ETGameTimerEvents::ET_onGameTick, dt);
}
