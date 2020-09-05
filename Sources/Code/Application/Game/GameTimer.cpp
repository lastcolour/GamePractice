#include "Game/GameTimer.hpp"
#include "Platform/ETSurface.hpp"
#include "Render/ETRenderCamera.hpp"

GameTimer::GameTimer() :
    skipUpdate(false),
    isPaused(true) {
}

GameTimer::~GameTimer() {
}

bool GameTimer::init() {
    ETNode<ETGameTimer>::connect(getEntityId());
    ETNode<ETGameUpdateTask>::connect(getEntityId());
    return true;
}

void GameTimer::deinit() {
    ETNode<ETGameTimer>::disconnect();
    ETNode<ETGameUpdateTask>::disconnect();
}

void GameTimer::ET_pauseTimer() {
    isPaused = true;
}

void GameTimer::ET_resumeTimer() {
    isPaused = false;
    skipUpdate = true;
}

void GameTimer::ET_updateGame() {
    ET_PollAllEvents<ETRenderCameraEvents>();
    ET_PollAllEvents<ETInputEvents>();

    if(isPaused) {
        return;
    }
    auto currTime = TimePoint::GetNowTime();
    auto dt = currTime.getSecondsElapsedFrom(lastTickT);
    if(skipUpdate) {
        dt = 0.f;
        skipUpdate = false;
    }
    lastTickT = currTime;

    ET_SendEvent(&ETGameTimerEvents::ET_onGameTick, dt);
}

bool GameTimer::ET_isTimerPaused() const {
    return isPaused;
}