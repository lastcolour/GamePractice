#include "Game/GameTimer.hpp"

GameTimer::GameTimer() :
    isPaused(true),
    skipUpdate(false) {
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
    skipUpdate = true;
}

void GameTimer::ET_onTick(float dt) {
    if(isPaused) {
        return;
    }
    if(skipUpdate) {
        dt = 0.f;
        skipUpdate = false;
    }
    ET_SendEvent(&ETGameTimerEvents::ET_onGameTick, dt);
}

bool GameTimer::ET_isTimerPaused() const {
    return isPaused;
}