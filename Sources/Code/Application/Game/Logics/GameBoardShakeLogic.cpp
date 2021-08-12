#include "Game/Logics/GameBoardShakeLogic.hpp"
#include "Game/ETGameBoardSpawner.hpp"

#include <cassert>

namespace {

const float SHAKE_EFFECT_INCREASE = 0.05f;
const float MAX_SHAKE_EFFECT_SCALE = 1.15f;

} // namespace

void GameBoardShakeLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardShakeLogic>("GameBoardShakeLogic")) {
        classInfo->addField("totalDuration", &GameBoardShakeLogic::totalDuration);
        classInfo->addField("shakes", &GameBoardShakeLogic::shakes);
    }
}

GameBoardShakeLogic::GameBoardShakeLogic() :
    totalDuration(0.1f),
    currentDuration(0.f),
    currentScale(1.f) {
}

GameBoardShakeLogic::~GameBoardShakeLogic() {
}

void GameBoardShakeLogic::init() {
    if(!shakes.empty()) {
        ETNode<ETGameBoardShake>::connect(getEntityId());
    }
    totalDuration = std::max(0.001f, totalDuration);
}

void GameBoardShakeLogic::deinit() {
}

void GameBoardShakeLogic::ET_addShake() {
    currentDuration = 0.f;
    currentScale = std::min(currentScale + SHAKE_EFFECT_INCREASE, MAX_SHAKE_EFFECT_SCALE);
    ETNode<ETGameTimerEvents>::connect(getEntityId());
}

void GameBoardShakeLogic::ET_onGameTick(float dt) {
    EntityId gameBoardId;
    ET_SendEventReturn(gameBoardId, &ETGameBoardSpawner::ET_getGameBoard);

    if(!gameBoardId.isValid()) {
        LogError("[GameBoardShakeLogic::ET_onGameTick] Can't get spawned game board");
        ETNode<ETGameTimerEvents>::disconnect();
        return;
    }

    currentDuration += dt;
    float perFrameDur = totalDuration / (shakes.size() + 1);
    int frameId = static_cast<int>(currentDuration / perFrameDur);
    frameId = std::min(frameId, static_cast<int>(shakes.size()));
    float currFrameT = currentDuration - frameId * perFrameDur;

    float t = std::min(1.f, currFrameT / perFrameDur);
    Vec2 resPt;
    if(frameId == 0) {
        resPt = Math::Lerp(Vec2(0.f), shakes[frameId], t);
    } else if(frameId < static_cast<int>(shakes.size())) {
        resPt = Math::Lerp(shakes[frameId - 1], shakes[frameId], t);
    } else {
        assert(frameId == static_cast<int>(shakes.size()) && "Invalid frame id");
        resPt = Math::Lerp(shakes[frameId - 1], Vec2(0.f), t);
    }

    int cellSize = 0;
    ET_SendEventReturn(cellSize, gameBoardId, &ETGameBoard::ET_getCellSize);

    resPt *= currentScale * cellSize;

    Transform tm;
    ET_SendEventReturn(tm, gameBoardId, &ETEntity::ET_getLocalTransform);

    if(currentDuration < totalDuration) {
        tm.pt = Vec3(resPt, 0.f);
        ET_SendEvent(gameBoardId, &ETEntity::ET_setLocalTransform, tm);
    } else {
        tm.pt = Vec3(0.f);
        ET_SendEvent(gameBoardId, &ETEntity::ET_setLocalTransform, tm);

        currentScale = 1.f;
        ETNode<ETGameTimerEvents>::disconnect();
    }
}