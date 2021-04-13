#include "Game/Logics/BoardElemMergeManager.hpp"
#include "Game/ETGameBoard.hpp"
#include "Render/ETParticlesSystem.hpp"
#include "Game/Logics/GameBoardUtils.hpp"

#include <cassert>

namespace {

Vec2 getRandomDir(Math::RandomFloatGenerator& floatGen) {
    auto x = floatGen.generate();
    auto y = floatGen.generate();
    auto dir = Vec2(x, y);
    return dir.getNormalized();
}

} // namespace

void BoardElemMergeManager::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<BoardElemMergeManager>("BoardElemMergeManager")) {
        classInfo->addField("mergeSound", &BoardElemMergeManager::mergeSound);
        classInfo->addField("mutateSound", &BoardElemMergeManager::mutateSound);
        classInfo->addField("startVel", &BoardElemMergeManager::startVel);
        classInfo->addField("acc", &BoardElemMergeManager::acc);
        classInfo->addField("mutateTaskDuration", &BoardElemMergeManager::mutateTaskDuration);
        classInfo->addField("mutateEffectId", &BoardElemMergeManager::mutateEffectId);
    }
}

BoardElemMergeManager::BoardElemMergeManager() :
    floatGen(-1.f, 1.f),
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
        if(!task.merges.empty()) {
            processMerges(task, dt, static_cast<float>(cellSize));
            ++it;
        } else {
            processMutate(task, dt);
            if(task.duration < 0.f) {
                it = mutateTasks.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void BoardElemMergeManager::processMutate(MutateTask& task, float dt) {
    task.duration -= dt;
    if(task.duration < 0.f) {
        ET_SendEvent(task.toId, &ETGameBoardElem::ET_onMergeDone);
        ET_SendEvent(&ETGameBoard::ET_replaceElemToSpecial, task.toId, task.elemType);
    }
}

void BoardElemMergeManager::processMerges(MutateTask& task, float dt, float cellSize) {
    Transform toTm;
    ET_SendEventReturn(toTm, task.toId, &ETEntity::ET_getTransform);

    Vec2 toPt(toTm.pt.x, toTm.pt.y);

    auto it = task.merges.begin();
    while(it != task.merges.end()) {
        Transform fromTm;
        ET_SendEventReturn(fromTm, it->targetId, &ETEntity::ET_getTransform);
        Vec2 fromPt(fromTm.pt.x, fromTm.pt.y);

        auto moveDir = toPt - fromPt;

        float distSq = moveDir.lenghtSq();
        moveDir.normalize();

        Vec2 offset = moveDir * it->vel * dt * cellSize;
        it->vel += acc * cellSize * dt;

        fromTm.pt.x += offset.x;
        fromTm.pt.y += offset.y;
        if(offset.lenghtSq() > distSq) {
            ET_SendEvent(it->targetId, &ETGameBoardElem::ET_onMergeDone);
            mergeSound.emit();
            it = task.merges.erase(it);
        } else {
            ET_SendEvent(it->targetId, &ETEntity::ET_setTransform, fromTm);
            ++it;
        }
    }
}

void BoardElemMergeManager::ET_createMergeTask(const ElemMergeTask& mergeTask) {
    assert(mergeTask.elems.size() && "Empty elems to merge");

    MutateTask mutateTask;
    mutateTask.toId = mergeTask.elems[0];
    mutateTask.duration = mutateTaskDuration;
    mutateTask.elemType = mergeTask.elemType;

    GameUtils::SetElemState(mutateTask.toId, EBoardElemState::Destroying);

    for(size_t i = 1, sz = mergeTask.elems.size(); i < sz; ++i) {
        auto& elemId = mergeTask.elems[i];
    
        GameUtils::SetElemState(elemId, EBoardElemState::Destroying);

        MergeTask task;
        task.targetId = elemId;
        task.vel = startVel;
        task.dir = getRandomDir(floatGen);
        mutateTask.merges.push_back(task);
    }

    mutateTasks.push_back(mutateTask);
}