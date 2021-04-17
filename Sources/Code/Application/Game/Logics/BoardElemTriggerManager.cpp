#include "Game/Logics/BoardElemTriggerManager.hpp"
#include "Game/Logics/GameBoardUtils.hpp"

#include <cassert>

void BoardElemTriggerManager::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<BoardElemTriggerManager>("BoardElemTriggerManager")) {
        classInfo->addField("triggerDelay", &BoardElemTriggerManager::triggerDelay);
    }
}

BoardElemTriggerManager::BoardElemTriggerManager() :
    triggerDelay(0.1f) {
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
    TriggerTask task;
    task.entId = elemId;
    task.delay = triggerDelay;
    task.destroyPlayed = false;
    newTriggerTasks.push_back(task);
    ET_SendEvent(elemId, &ETGameBoardElemTriggerLogic::ET_start);
}

void BoardElemTriggerManager::ET_updateTriggerTasks(float dt) {
    triggerTasks.insert(triggerTasks.end(), newTriggerTasks.begin(), newTriggerTasks.end());
    newTriggerTasks.clear();
    auto it = triggerTasks.begin();
    while(it != triggerTasks.end()) {
        it->delay -= dt;
        if(it->delay > 0.f) {
            break;
        }
        if(!it->destroyPlayed) {
            it->destroyPlayed = true;
            GameUtils::PlayElemDestroyEffect(it->entId);
        }
        bool isEnded = true;
        ET_SendEventReturn(isEnded, it->entId, &ETGameBoardElemTriggerLogic::ET_update, dt);
        if(isEnded) {
            it = triggerTasks.erase(it);
        } else {
            break;
        }
    }
}

bool BoardElemTriggerManager::ET_hasTriggerTasks() const {
    return !triggerTasks.empty() || !newTriggerTasks.empty();
}