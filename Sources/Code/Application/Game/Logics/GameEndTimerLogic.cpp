#include "Game/Logics/GameEndTimerLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Reflect/ReflectContext.hpp"

#include <algorithm>

GameEndTimerLogic::GameEndTimerLogic() :
    endTime(120.f),
    remainingTime(0.f) {
}

GameEndTimerLogic::~GameEndTimerLogic() {
}

void GameEndTimerLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameEndTimerLogic>("GameEndTimer")) {
        classInfo->addField("gameDuration", &GameEndTimerLogic::endTime);
    }
}

void GameEndTimerLogic::init() {
    remainingTime = endTime;
    ETNode<ETGameEndTimer>::connect(getEntityId());

    ET_SendEvent(&ETGameEndTimerUpdater::ET_setEndTime, std::max(remainingTime, 0.f));
}

void GameEndTimerLogic::deinit() {
}

void GameEndTimerLogic::ET_startEndTimer() {
    remainingTime = endTime;
    ET_SendEvent(&ETGameEndTimerUpdater::ET_setEndTime, std::max(remainingTime, 0.f));
    ETNode<ETGameTimerEvents>::connect(getEntityId());
}

void GameEndTimerLogic::ET_onGameTick(float dt) {
    remainingTime -= dt;
    ET_SendEvent(&ETGameEndTimerUpdater::ET_setEndTime, std::max(remainingTime, 0.f));
    if(remainingTime < 0.f) {
        ET_SendEvent(&ETGameEndTimerEvents::ET_onGameTimeOut);
        ETNode<ETGameTimerEvents>::disconnect();
    }
}

float GameEndTimerLogic::ET_getRemainingTime() const {
    return remainingTime;
}

float GameEndTimerLogic::ET_getInitialEndGameDuration() const {
    return endTime;
}