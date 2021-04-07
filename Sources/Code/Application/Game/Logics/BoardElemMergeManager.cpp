#include "Game/Logics/BoardElemMergeManager.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/ETGameElem.hpp"
#include "Render/ETParticlesSystem.hpp"

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
        classInfo->addResourceField("vRocketEntity", ResourceType::Entity, &BoardElemMergeManager::vRocketEntity);
        classInfo->addResourceField("hRocketEntity", ResourceType::Entity, &BoardElemMergeManager::hRocketEntity);
        classInfo->addResourceField("bombEntity", ResourceType::Entity, &BoardElemMergeManager::bombEntity);
        classInfo->addResourceField("starEntity", ResourceType::Entity, &BoardElemMergeManager::starEntity);
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
    return !tasksMap.empty()
        || !mutateTasks.empty();
}

void BoardElemMergeManager::ET_updateMergeTasks(float dt) {
    int cellSize = 0;
    ET_SendEventReturn(cellSize, &ETGameBoard::ET_getCellSize);

    auto it = tasksMap.begin();
    while(it != tasksMap.end()) {
        processMergeTasks(it->first, it->second, dt, static_cast<float>(cellSize));
        if(it->second.empty()) {

            mutateSound.emit();

            Transform tm;
            ET_SendEventReturn(tm, it->first, &ETEntity::ET_getTransform);
            ET_SendEvent(mutateEffectId, &ETParticlesSystem::ET_emitWithTm, tm);

            MutateTask task;
            task.targetId = it->first;
            task.duration = mutateTaskDuration;

            mutateTasks.push_back(task);

            it = tasksMap.erase(it);
        } else {
            ++it;
        }
    }

    processMutateTasks(dt);
}

void BoardElemMergeManager::processMutateTasks(float dt) {
    auto it = mutateTasks.begin();
    while(it != mutateTasks.end()) {
        it->duration -= dt;
        if(it->duration < 0.f) {

            ET_SendEvent(it->targetId, &ETGameBoardElem::ET_triggerMutate);

            it = mutateTasks.erase(it);
        } else {
            ++it;
        }
    }
}

void BoardElemMergeManager::processMergeTasks(EntityId toId, std::vector<MergeTask>& tasks, float dt, float cellSize) {
    Transform toTm;
    ET_SendEventReturn(toTm, toId, &ETEntity::ET_getTransform);

    Vec2 toPt(toTm.pt.x, toTm.pt.y);

    auto it = tasks.begin();
    while(it != tasks.end()) {
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
            ET_SendEvent(it->targetId, &ETGameBoardElem::ET_onMergeDone, toId);
            mergeSound.emit();
            it = tasks.erase(it);
        } else {
            ET_SendEvent(it->targetId, &ETEntity::ET_setTransform, fromTm);
            ++it;
        }
    }
}

void BoardElemMergeManager::ET_createMergeTask(EntityId fromId, EntityId toId) {
    assert(fromId.isValid() && "Invalid from id");
    assert(toId.isValid() && "Invalid from id");

    MergeTask task;
    task.targetId = fromId;
    task.dir = getRandomDir(floatGen);
    task.vel = startVel;

    auto it = tasksMap.find(toId);
    if(it != tasksMap.end()) {
        it->second.push_back(task);
    } else {
        tasksMap[toId].push_back(task);
    }
}