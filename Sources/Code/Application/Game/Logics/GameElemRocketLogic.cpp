#include "Game/Logics/GameElemRocketLogic.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/Logics/GameBoardUtils.hpp"
#include "Render/ETRenderNode.hpp"
#include "Render/ETParticlesSystem.hpp"
#include "UI/ETUIViewPort.hpp"

namespace {

bool isMoveEnded(const Vec3& pt, int cellSize, const Vec2i& startPt, const Vec2i& boardSize, float fOffset, const Vec2i& moveDir) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    AABB2D viewBox;
    viewBox.top = Vec2(static_cast<float>(viewPort.x + cellSize),
        static_cast<float>(viewPort.y + cellSize));
    viewBox.bot = Vec2(static_cast<float>(-cellSize));
    bool isOutOfViewPort = !viewBox.isInside(Vec2(pt.x, pt.y));
    bool isOutOfBoard = false;

    int iOffset = static_cast<int>(fOffset) + 1;
    Vec2i boardPt = moveDir * iOffset + startPt;
    if(boardPt.x < 0 || boardPt.x >= boardSize.x) {
        isOutOfBoard = true;
    }
    if(boardPt.y < 0 || boardPt.y >= boardSize.y) {
        isOutOfBoard = true;
    }

    return isOutOfBoard && isOutOfViewPort;
}

bool moveRocket(EntityId rocketId, Vec2i moveDir, const Transform& tm, float fOffset,
    int cellSize, const Vec2i& startPt, const Vec2i& boardSize) {
    Transform rocketTm = tm;
    rocketTm.pt += (fOffset * cellSize) * Vec3(
        static_cast<float>(moveDir.x), static_cast<float>(moveDir.y), 0.f);
    ET_SendEvent(rocketId, &ETEntity::ET_setTransform, rocketTm);

    bool isEnded = isMoveEnded(rocketTm.pt, cellSize, startPt, boardSize, fOffset, moveDir);

    if(isEnded) {
        ET_SendEvent(rocketId, &ETRenderNode::ET_hide);
        EntityId trailEffectId;
        ET_SendEventReturn(trailEffectId, &ETGameBoardEffects::ET_getRocketTrailEffect);
        ET_SendEvent(trailEffectId, &ETParticlesSystem::ET_stopTrackedEmitter, rocketId);
    }

    return isEnded;
}

} // namespace

void GameElemRocketLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameElemRocketLogic>("GameElemRocketLogic")) {
        classInfo->addField("speed", &GameElemRocketLogic::speed);
        classInfo->addField("acc", &GameElemRocketLogic::acc);
        classInfo->addField("firstRocket", &GameElemRocketLogic::firstRocket);
        classInfo->addField("secondRocket", &GameElemRocketLogic::secondRocket);
    }
}

GameElemRocketLogic::GameElemRocketLogic() :
    startPt(-1),
    speed(1.f),
    currTime(0.f),
    acc(0.1f),
    currSpeed(0.f),
    prevOffset(0),
    isHorizontal(false),
    isStarted(false) {
}

GameElemRocketLogic::~GameElemRocketLogic() {
}

void GameElemRocketLogic::init() {
    ETNode<ETGameBoardElemTriggerLogic>::connect(getEntityId());
    ETNode<ETGameBoardRenderElem>::connect(getEntityId());

    ET_SendEvent(firstRocket, &ETRenderNode::ET_hide);
    ET_SendEvent(secondRocket, &ETRenderNode::ET_hide);
}

void GameElemRocketLogic::deinit() {
}

void GameElemRocketLogic::ET_start() {
    isStarted = false;
    prevOffset = 0;
    currTime = 0.f;
    currSpeed = speed;
    ET_SendEventReturn(startPt, &ETGameBoard::ET_getElemBoardPos, getEntityId());

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);

    ET_SendEvent(firstRocket, &ETEntity::ET_setTransform, tm);
    ET_SendEvent(secondRocket, &ETEntity::ET_setTransform, tm);

    auto elemType = GameUtils::GetElemType(getEntityId());
    if(elemType == EBoardElemType::HRocket) {
        isHorizontal = true;
    } else {
        isHorizontal = false;
    }
}

void GameElemRocketLogic::ET_setSwapedElem(EntityId elemId) {
}

bool GameElemRocketLogic::ET_update(float dt) {
    if(!isStarted) {
        isStarted = true;
        onStart();
    }
    auto isEnded = onUpdate(dt);
    if(isEnded) {
        ET_SendEvent(getEntityId(), &ETGameBoardElem::ET_onTriggerDone);
    }
    return isEnded;
}

void GameElemRocketLogic::onStart() {
    ET_SendEvent(getEntityId(), &ETRenderNode::ET_hide);

    ET_SendEvent(firstRocket, &ETRenderNode::ET_show);
    ET_SendEvent(secondRocket, &ETRenderNode::ET_show);

    EntityId trailEffectId;
    ET_SendEventReturn(trailEffectId, &ETGameBoardEffects::ET_getRocketTrailEffect);

    ET_SendEvent(trailEffectId, &ETParticlesSystem::ET_emitTrackingEntity, firstRocket);
    ET_SendEvent(trailEffectId, &ETParticlesSystem::ET_emitTrackingEntity, secondRocket);
}

bool GameElemRocketLogic::onUpdate(float dt) {
    currTime += dt;

    Vec2i dir = isHorizontal ? Vec2i(1, 0) : Vec2i(0, 1);

    int cellSize = 0;
    ET_SendEventReturn(cellSize, &ETGameBoard::ET_getCellSize);

    Vec2i boardSize(0);
    ET_SendEventReturn(boardSize, &ETGameBoard::ET_getBoardSize);

    float fOffset = currSpeed * currTime;
    currSpeed += acc * dt;

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);

    tm.quat.setAxisAngle(Vec3(0.f, 0.f, 1.f), 
        isHorizontal ? -Math::PI / 2.f : 0.f);

    auto isFirstDone = moveRocket(firstRocket, dir, tm, fOffset, cellSize, startPt, boardSize);

    tm.quat.setAxisAngle(Vec3(0.f, 0.f, 1.f), 
        isHorizontal ? Math::PI / 2.f : Math::PI);

    auto isSecondDone = moveRocket(secondRocket, -dir, tm, fOffset, cellSize, startPt, boardSize);

    int iOffset = static_cast<int>(fOffset) + 2;
    for(int i = prevOffset; i < iOffset; ++i) {
        {
            Vec2i pt = startPt;
            pt += dir * i;
            GameUtils::TryTriggerElemDestroy(getEntityId(), pt);
        }
        {
            Vec2i pt = startPt;
            pt -= dir * i;
            GameUtils::TryTriggerElemDestroy(getEntityId(), pt);
        }
    }
    prevOffset = iOffset;

    return isFirstDone && isSecondDone;
}

void GameElemRocketLogic::ET_initRender(UIProxyContainer& rootContainer, const Vec2& elemSize) {
    ET_SendEvent(firstRocket, &ETRenderRect::ET_setSize, elemSize);
    ET_SendEvent(firstRocket, &ETRenderNode::ET_hide);
    rootContainer.addItem(firstRocket, GameUtils::ROCKET_PART_Z_OFFSET);

    ET_SendEvent(secondRocket, &ETRenderRect::ET_setSize, elemSize);
    ET_SendEvent(secondRocket, &ETRenderNode::ET_hide);
    rootContainer.addItem(secondRocket, GameUtils::ROCKET_PART_Z_OFFSET);
}

void GameElemRocketLogic::ET_deinitRender(UIProxyContainer& rootContainer) {
    rootContainer.removeItem(firstRocket);
    ET_SendEvent(firstRocket, &ETRenderNode::ET_hide);

    rootContainer.removeItem(secondRocket);
    ET_SendEvent(secondRocket, &ETRenderNode::ET_hide);
}