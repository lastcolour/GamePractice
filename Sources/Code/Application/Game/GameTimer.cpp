#include "Game/GameTimer.hpp"
#include "Platform/ETSurface.hpp"
#include "Render/ETRenderCamera.hpp"

GameTimer::GameTimer() :
    scale(1.f),
    skipUpdate(false),
    isPaused(true) {
}

GameTimer::~GameTimer() {
}

bool GameTimer::init() {
    ETNode<ETGameTimer>::connect(getEntityId());
    return true;
}

void GameTimer::deinit() {
    ETNode<ETGameTimer>::disconnect();
}

void GameTimer::ET_onTick() {
    ET_PollAllEvents<ETGameTimer>();
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
    dt *= scale;
    ET_SendEvent(&ETGameTimerEvents::ET_onGameTick, dt);
}

void GameTimer::ET_setScale(float newScale) {
    scale = newScale;
}

void GameTimer::ET_pause() {
    isPaused = true;
}

void GameTimer::ET_resume() {
    isPaused = false;
    skipUpdate = true;
}

bool GameTimer::ET_isPaused() const {
    return isPaused;
}