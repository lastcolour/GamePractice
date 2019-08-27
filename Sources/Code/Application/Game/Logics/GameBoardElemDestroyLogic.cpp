#include "Game/Logics/GameBoardElemDestroyLogic.hpp"
#include "Entity/ETEntityInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include <algorithm>

GameBoardElemDestroyLogic::GameBoardElemDestroyLogic() {
}

GameBoardElemDestroyLogic::~GameBoardElemDestroyLogic() {
}

bool GameBoardElemDestroyLogic::serialize(const JSONNode& node) {
    node.read("destroyDuration", destroyDuration);
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
        ET_SendEventReturn(tm, task.entId, &ETEntity::ET_getTransform);
        tm.scale = Math::Lerp(task.startScale, Vec3(0.f), prog);

        ET_SendEvent(task.entId, &ETEntity::ET_setTransform, tm);
    }

    int destroyedCount = 0;
    auto it = destroyTasks.begin();
    while(it != destroyTasks.end()) {
        auto& task = *it;
        if(task.duration > destroyDuration) {
            ET_SendEvent(getEntityId(), &ETGameBoard::ET_setElemState, task.entId, EBoardElemState::Void);
            it = destroyTasks.erase(it);
            ++destroyedCount;
        } else {
            ++it;
        }
    }

    if(destroyedCount > 0) {
        ET_SendEvent(getEntityId(), &ETGameBoard::ET_updateBoard);
        ET_SendEvent(getEntityId(), &ETGameScore::ET_onElemsDestroyed, destroyedCount);
    }
}

void GameBoardElemDestroyLogic::ET_destroyBoardElem(EntityId elemId) {
    ET_SendEvent(getEntityId(), &ETGameBoard::ET_setElemState, elemId, EBoardElemState::Removing);

    DestroyTask task;
    task.entId = elemId;
    task.duration = 0.f;

    Transform tm;
    ET_SendEventReturn(tm, task.entId, &ETEntity::ET_getTransform);
    task.startScale = tm.scale;

    destroyTasks.push_back(task);
}