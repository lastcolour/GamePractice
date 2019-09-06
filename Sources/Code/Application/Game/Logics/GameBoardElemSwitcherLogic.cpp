#include "Game/Logics/GameBoardElemSwitcherLogic.hpp"
#include "Entity/ETEntityInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include <algorithm>
#include <cassert>

GameBoardElemSwitcherLogic::GameBoardElemSwitcherLogic() :
    switchDuration(1.f) {
}

GameBoardElemSwitcherLogic::~GameBoardElemSwitcherLogic() {
}

bool GameBoardElemSwitcherLogic::serialize(const JSONNode& node) {
    node.read("switchDuration", switchDuration);
    return true;
}

bool GameBoardElemSwitcherLogic::init() {
    ETNode<ETGameTimerEvents>::connect(getEntityId());
    ETNode<ETGameBoardElemSwitcher>::connect(getEntityId());
    return true;
}

void GameBoardElemSwitcherLogic::ET_onGameTick(float dt) {
    for(auto& task : switchTasks) {
        task.duration += dt;

        float prog = std::min(task.duration / switchDuration, 1.f);

        Transform newTm = task.firstTm;
        newTm.pt = Math::Lerp(task.firstTm.pt, task.secondTm.pt, prog);
        ET_SendEvent(task.firstId, &ETEntity::ET_setTransform, newTm);

        newTm = task.secondTm;
        newTm.pt = Math::Lerp(task.secondTm.pt, task.firstTm.pt, prog);
        ET_SendEvent(task.secondId, &ETEntity::ET_setTransform, newTm);
    }

    bool isNeedUpdateBoard = false;
    auto it = switchTasks.begin();
    while(it != switchTasks.end()) {
        auto& task = *it;
        if(task.duration > switchDuration) {

            ET_SendEvent(getEntityId(), &ETGameBoard::ET_switchElemsBoardPos, task.firstId, task.secondId);
            ET_SendEvent(getEntityId(), &ETGameBoard::ET_setElemState, task.firstId, EBoardElemState::Static);
            ET_SendEvent(getEntityId(), &ETGameBoard::ET_setElemState, task.secondId, EBoardElemState::Static);
            isNeedUpdateBoard = true;
            it = switchTasks.erase(it);
        } else {
            ++it;
        }
    }

    if(isNeedUpdateBoard) {
        ET_SendEvent(getEntityId(), &ETGameBoard::ET_updateBoard);
    }
}

void GameBoardElemSwitcherLogic::ET_switchBoardElems(EntityId firstId, EntityId secondId) {
    assert(firstId != secondId && "Can't switch same element");

    SwitchTask task;
    task.duration = 0.f;

    task.firstId = firstId;
    ET_SendEventReturn(task.firstTm, firstId, &ETEntity::ET_getTransform);
    ET_SendEvent(getEntityId(), &ETGameBoard::ET_setElemState, firstId, EBoardElemState::Switching);

    task.secondId = secondId;
    ET_SendEventReturn(task.secondTm, secondId, &ETEntity::ET_getTransform);
    ET_SendEvent(getEntityId(), &ETGameBoard::ET_setElemState, secondId, EBoardElemState::Switching);

    switchTasks.push_back(task);
}