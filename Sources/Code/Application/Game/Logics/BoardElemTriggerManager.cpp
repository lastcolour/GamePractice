#include "Game/Logics/BoardElemTriggerManager.hpp"

#include <cassert>

void BoardElemTriggerManager::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<BoardElemTriggerManager>("BoardElemTriggerManager")) {
        
    }
}

BoardElemTriggerManager::BoardElemTriggerManager() {
}

BoardElemTriggerManager::~BoardElemTriggerManager() {
}

void BoardElemTriggerManager::init() {
    ETNode<ETGameBoardElemTriggerManager>::connect(getEntityId());
}

void BoardElemTriggerManager::deinit() {
}

void BoardElemTriggerManager::ET_createTriggerTask(EntityId elemId) {
    if(!elemId.isValid()) {
        assert(false && "Invalid elem type");
        return;
    }
    triggerTasks.push_back(elemId);
    ET_SendEvent(elemId, &ETGameBoardElemTriggerLogic::ET_start);
}

void BoardElemTriggerManager::ET_updateTriggerTasks(float dt) {
    auto it = triggerTasks.begin();
    while(it != triggerTasks.end()) {
        bool isEnded = true;
        ET_SendEventReturn(isEnded, &ETGameBoardElemTriggerLogic::ET_update, dt);
        if(isEnded) {
            it = triggerTasks.erase(it);
        } else {
            ++it;
        }
    }
}

bool BoardElemTriggerManager::ET_hasTriggerTasks() const {
    return !triggerTasks.empty();
}