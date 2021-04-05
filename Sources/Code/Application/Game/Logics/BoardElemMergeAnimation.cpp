#include "Game/Logics/BoardElemMergeAnimation.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/ETGameElem.hpp"

#include <cassert>

namespace {

Vec2 getRandomDir(Math::RandomFloatGenerator& floatGen) {
    auto x = floatGen.generate();
    auto y = floatGen.generate();
    auto dir = Vec2(x, y);
    return dir.getNormalized();
}

} // namespace

void BoardElemMergeAnimation::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<BoardElemMergeAnimation>("BoardElemMergeAnimation")) {
        classInfo->addField("mergeSound", &BoardElemMergeAnimation::mergeSound);
        classInfo->addField("startVel", &BoardElemMergeAnimation::startVel);
        classInfo->addField("acc", &BoardElemMergeAnimation::acc);
    }
}

BoardElemMergeAnimation::BoardElemMergeAnimation() :
    floatGen(-1.f, 1.f),
    startVel(1.f),
    acc(1.f) {
}

BoardElemMergeAnimation::~BoardElemMergeAnimation() {
}

void BoardElemMergeAnimation::init() {
    startVel = std::max(0.001f, startVel);
    acc = std::max(0.001f, acc);

    ETNode<ETGameBoardElemMergeAnimationManager>::connect(getEntityId());
    ETNode<ETGameTimerEvents>::connect(getEntityId());
}

void BoardElemMergeAnimation::deinit() {
}

void BoardElemMergeAnimation::ET_onGameTick(float dt) {
    int cellSize = 0;
    ET_SendEventReturn(cellSize, &ETGameBoard::ET_getCellSize);

    auto it = tasksMap.begin();
    while(it != tasksMap.end()) {
        processMergeTasks(it->first, it->second, dt, static_cast<float>(cellSize));
        if(it->second.empty()) {
            it = tasksMap.erase(it);
        } else {
            ++it;
        }
    }
}

void BoardElemMergeAnimation::processMergeTasks(EntityId toId, std::vector<MergeTask>& tasks, float dt, float cellSize) {
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
            LogError("[tmpLog] ET_onMergeDone");
            ET_SendEvent(it->targetId, &ETGameBoardElem::ET_onMergeDone, toId);
            it = tasks.erase(it);
        } else {
            ET_SendEvent(it->targetId, &ETEntity::ET_setTransform, fromTm);
            ++it;
        }
    }
}

void BoardElemMergeAnimation::ET_createMergeTask(EntityId fromId, EntityId toId) {
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