#include "Game/Logics/BoardElemMergeManager.hpp"
#include "Game/ETGameBoard.hpp"
#include "Render/ETParticlesSystem.hpp"
#include "Game/Logics/GameBoardUtils.hpp"

#include <cassert>

namespace {

const float MAX_PULSE_SCALE = 1.6f;
const float MIN_SHRINK_SIZE = 0.3f;

} // namespace

void BoardElemMergeManager::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<BoardElemMergeManager>("BoardElemMergeManager")) {
        classInfo->addField("startVel", &BoardElemMergeManager::startVel);
        classInfo->addField("acc", &BoardElemMergeManager::acc);
        classInfo->addField("pulseScaleIncrese", &BoardElemMergeManager::pulseScaleIncrese);
        classInfo->addField("pulseScaleDecaySpeed", &BoardElemMergeManager::pulseScaleDecaySpeed);
        classInfo->addField("shrinkSpeed", &BoardElemMergeManager::shrinkSpeed);
        classInfo->addField("mutateTaskDuration", &BoardElemMergeManager::mutateTaskDuration);
    }
}

BoardElemMergeManager::BoardElemMergeManager() :
    pulseScaleIncrese(0.4f),
    pulseScaleDecaySpeed(5.f),
    shrinkSpeed(1.f),
    startVel(1.f),
    acc(1.f),
    mutateTaskDuration(0.2f) {
}

BoardElemMergeManager::~BoardElemMergeManager() {
}

void BoardElemMergeManager::init() {
    startVel = std::max(0.001f, startVel);
    acc = std::max(0.001f, acc);

    ETNode<ETGameBoardElemMergeManager>::connect(getEntityId());
    ETNode<ETGameBoardSpawnerEvents>::connect(getEntityId());
}

void BoardElemMergeManager::deinit() {
}

bool BoardElemMergeManager::ET_hasMergeTasks() const  {
    return !mutateTasks.empty();
}

void BoardElemMergeManager::ET_updateMergeTasks(float dt) {
    int cellSize = 0;
    ET_SendEventReturn(cellSize, &ETGameBoard::ET_getCellSize);

    auto it = mutateTasks.begin();
    while(it != mutateTasks.end()) {
        auto& task = *it;
        processMerges(task, dt, static_cast<float>(cellSize));
        if(task.merges.empty() && task.duration < 0.f) {
            it = mutateTasks.erase(it);
        } else {
            ++it;
        }
    }
}

void BoardElemMergeManager::processMerges(MutateTask& task, float dt, float cellSize) {
    Transform toTm;
    ET_SendEventReturn(toTm, task.toId, &ETEntity::ET_getLocalTransform);

    Vec2 toPt(toTm.pt.x, toTm.pt.y);

    auto it = task.merges.begin();
    while(it != task.merges.end()) {
        Transform fromTm;
        ET_SendEventReturn(fromTm, it->targetId, &ETEntity::ET_getLocalTransform);
        Vec2 fromPt(fromTm.pt.x, fromTm.pt.y);

        auto moveDir = toPt - fromPt;

        float distSq = moveDir.lengthSq();
        moveDir.normalize();

        Vec2 offset = moveDir * it->vel * dt * cellSize;
        it->vel += acc * cellSize * dt;

        fromTm.pt.x += offset.x;
        fromTm.pt.y += offset.y;

        fromTm.scale -= Vec3(shrinkSpeed * dt);
        if(fromTm.scale.max() < MIN_SHRINK_SIZE) {
            fromTm.scale = Vec3(MIN_SHRINK_SIZE);
        }

        if(offset.lengthSq() > distSq) {
            ET_SendEvent(&ETGameBoardEffects::ET_playMergeEffect, task.toId);
            ET_SendEvent(it->targetId, &ETGameBoardElem::ET_onMergeDone);
            it = task.merges.erase(it);
            task.pulseScale = std::min(MAX_PULSE_SCALE, task.pulseScale + pulseScaleIncrese);
        } else {
            ET_SendEvent(it->targetId, &ETEntity::ET_setLocalTransform, fromTm);
            ++it;
        }
    }

    task.pulseScale = std::max(1.f, task.pulseScale -  dt * pulseScaleDecaySpeed);

    toTm.scale = Vec3(task.pulseScale);
    ET_SendEvent(task.toId, &ETEntity::ET_setLocalTransform, toTm);

    if(!task.merges.empty()) {
        return;
    }

    task.duration -= dt;
    if(task.duration <= 0.f) {

        toTm.scale = Vec3(1.f);
        ET_SendEvent(task.toId, &ETEntity::ET_setLocalTransform, toTm);

        ET_SendEvent(&ETGameBoardEffects::ET_playMutateEffect, task.toId);
        ET_SendEvent(task.toId, &ETGameBoardElem::ET_onMergeDone);

        ET_SendEvent(&ETGameBoard::ET_replaceElemToSpecial, task.toId, task.elemType);
    }
}

void BoardElemMergeManager::ET_createMergeTask(const ElemMergeTask& mergeTask) {
    assert(mergeTask.elems.size() && "Empty elems to merge");

    MutateTask mutateTask;
    mutateTask.toId = mergeTask.elems[0];
    mutateTask.duration = mutateTaskDuration;
    mutateTask.elemType = mergeTask.elemType;
    mutateTask.pulseScale = 1.f;

    GameUtils::SetElemState(mutateTask.toId, EBoardElemState::Destroying);

    for(size_t i = 1, sz = mergeTask.elems.size(); i < sz; ++i) {
        auto& elemId = mergeTask.elems[i];
    
        GameUtils::SetElemState(elemId, EBoardElemState::Destroying);

        MergeTask task;
        task.targetId = elemId;
        task.vel = startVel;
        mutateTask.merges.push_back(task);
    }

    mutateTasks.push_back(mutateTask);
}

void BoardElemMergeManager::ET_onStartLoading() {
    assert(!ET_hasMergeTasks() && "Mutate tasks already exists");
}

void BoardElemMergeManager::ET_onStartDestroying() {
    mutateTasks.clear();
}