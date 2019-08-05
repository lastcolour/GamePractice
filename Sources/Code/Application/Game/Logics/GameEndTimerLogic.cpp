#include "Game/Logics/GameEndTimerLogic.hpp"
#include "UI/UIEventManager.hpp"
#include "Core/JSONNode.hpp"

GameEndTimerLogic::GameEndTimerLogic() :
    endTime(0.f),
    remainingTime(0.f) {
}

GameEndTimerLogic::~GameEndTimerLogic() {
}

bool GameEndTimerLogic::serialize(const JSONNode& node) {
    node.value("time", endTime);
    return true;
}

bool GameEndTimerLogic::init() {
    remainingTime = endTime;
    ETNode<ETTimerEvents>::connect(getEntityId());
    return true;
}

void GameEndTimerLogic::ET_onTick(float dt) {
    remainingTime -= dt;
    if(remainingTime < 0.f) {
        ETNode<ETTimerEvents>::disconnect();
        ET_SendEvent(&ETUIEventManager::ET_onEvent, "Game_OnGameEnd");
    }
}
