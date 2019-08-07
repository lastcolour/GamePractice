#include "Game/Logics/GameBoardElemDestroyLogic.hpp"
#include "Core/JSONNode.hpp"

#include <algorithm>

GameBoardElemDestroyLogic::GameBoardElemDestroyLogic() {
}

GameBoardElemDestroyLogic::~GameBoardElemDestroyLogic() {
}

bool GameBoardElemDestroyLogic::serialize(const JSONNode& node) {
    node.value("destroyDuration", destroyDuration);
    return true;
}

bool GameBoardElemDestroyLogic::init() {
    ETNode<ETTimerEvents>::connect(getEntityId());
    ETNode<ETGameBoardElemDestroy>::connect(getEntityId());
    return true;
}

void GameBoardElemDestroyLogic::ET_onTick(float dt) {
    for(auto& task : destroyTasks) {
        task.duration += dt;
        float prog = std::min(task.duration / destroyDuration, 1.f);
        
        Transform tm;
        ET_SendEventReturn(tm, task.entId, &ETGameObject::ET_getTransform);
        tm.scale = Math::Lerp(task.startScale, Vec3(0.f), prog);

        ET_SendEvent(task.entId, &ETGameObject::ET_setTransform, tm);
    }

    bool isNeedUpdateBoard = false;
    auto it = destroyTasks.begin();
    while(it != destroyTasks.end()) {
        auto& task = *it;
        if(task.duration > destroyDuration) {
            ET_SendEvent(getEntityId(), &ETGameBoard::ET_setElemState, task.entId, EBoardElemState::Void);
            it = destroyTasks.erase(it);
            isNeedUpdateBoard = true;
        } else {
            ++it;
        }
    }

    if(isNeedUpdateBoard) {
        ET_SendEvent(getEntityId(), &ETGameBoard::ET_updateBoard);
    }
}

void GameBoardElemDestroyLogic::ET_destroyBoardElem(EntityId elemId) {
    ET_SendEvent(getEntityId(), &ETGameBoard::ET_setElemState, elemId, EBoardElemState::Removing);

    DestroyTask task;
    task.entId = elemId;
    task.duration = 0.f;

    Transform tm;
    ET_SendEventReturn(tm, task.entId, &ETGameObject::ET_getTransform);
    task.startScale = tm.scale;

    destroyTasks.push_back(task);
}