#include "Game/Logics/GameEndTimerLogic.hpp"
#include "UI/UIEventManager.hpp"
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
    node.read("event", endEvent);
    return true;
}

bool GameEndTimerLogic::init() {
    remainingTime = endTime;
    ETNode<ETTimerEvents>::connect(getEntityId());
    return true;
}

void GameEndTimerLogic::ET_onTick(float dt) {
    remainingTime -= dt;
    ET_SendEvent(&ETGameEndTimerUpdater::ET_setEndTime, std::max(remainingTime, 0.f));
    if(remainingTime < 0.f) {
        ETNode<ETTimerEvents>::disconnect();
        ET_SendEvent(&ETUIEventManager::ET_onEvent, endEvent.c_str());
    }
}
