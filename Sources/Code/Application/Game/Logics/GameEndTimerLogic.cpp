#include "Game/Logics/GameEndTimerLogic.hpp"
#include "Core/JSONNode.hpp"
#include "Game/ETGameInterfaces.hpp"

#include <algorithm>

GameEndTimerLogic::GameEndTimerLogic() :
    endTime(120.f),
    remainingTime(0.f) {
}

GameEndTimerLogic::~GameEndTimerLogic() {
}

bool GameEndTimerLogic::serialize(const JSONNode& node) {
    node.read("time", endTime);
    return true;
}

bool GameEndTimerLogic::init() {
    remainingTime = endTime;
    ETNode<ETGameTimerEvents>::connect(getEntityId());
    ETNode<ETGameEndTimer>::connect(getEntityId());

    ET_SendEvent(&ETGameEndTimerUpdater::ET_setEndTime, std::max(remainingTime, 0.f));
    return true;
}

void GameEndTimerLogic::ET_onGameTick(float dt) {
    remainingTime -= dt;
    ET_SendEvent(&ETGameEndTimerUpdater::ET_setEndTime, std::max(remainingTime, 0.f));
    if(remainingTime < 0.f) {
        //ET_SendEvent(&ETGameStateManager::ET_endGame, EEndGameReason::TimeOut);
        ETNode<ETGameTimerEvents>::disconnect();
    }
}

float GameEndTimerLogic::ET_getRemainingTime() const {
    return remainingTime;
}

float GameEndTimerLogic::ET_getInitialEndGameDuration() const {
    return endTime;
}