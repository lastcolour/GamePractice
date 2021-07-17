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
    ETNode<ETGameBoardSpawnerEvents>::connect(getEntityId());
}

void BoardElemTriggerManager::deinit() {
}

void BoardElemTriggerManager::ET_createTriggerTask(EntityId elemId, bool applyDelay) {
    if(!elemId.isValid()) {
        assert(false && "Invalid elem type");
        return;
    }
    TriggerTask task;
    task.entId = elemId;
    task.delay = applyDelay ? triggerDelay : 0.f;
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
        bool isEnded = false;
        if(it->delay < 0.f) {
            if(!it->destroyPlayed) {
                it->destroyPlayed = true;
                GameUtils::PlayElemDestroyEffect(it->entId);
            }
            ET_SendEventReturn(isEnded, it->entId, &ETGameBoardElemTriggerLogic::ET_update, dt);
        }
        if(isEnded) {
            it = triggerTasks.erase(it);
        } else {
            ++it;
        }
    }
}

bool BoardElemTriggerManager::ET_hasTriggerTasks() const {
    return !triggerTasks.empty() || !newTriggerTasks.empty();
}

void BoardElemTriggerManager::ET_onStartLoading() {
    assert(!ET_hasTriggerTasks() && "Invalid trigger tasks");
}

void BoardElemTriggerManager::ET_onStartDestroying() {
    triggerTasks.clear();
    newTriggerTasks.clear();
}