#include "Game/Logics/GameBoardAppearAnimationLogic.hpp"
#include "Entity/ETEntityInterfaces.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "Core/JSONNode.hpp"
#include "Render/ETRenderInterfaces.hpp"

#include <algorithm>

GameBoardAppearAnimationLogic::GameBoardAppearAnimationLogic() :
    boardSize(0),
    currDuration(0.f),
    fallSpeed(1.f),
    shift(0.f),
    collumnDelay(0.2f),
    duration(1.f) {
}

GameBoardAppearAnimationLogic::~GameBoardAppearAnimationLogic() {
}

bool GameBoardAppearAnimationLogic::serialize(const JSONNode& node) {
    node.read("fallSpeed", fallSpeed);
    node.read("collumnDelay", collumnDelay);
    node.read("duration", duration);
    return true;
}

bool GameBoardAppearAnimationLogic::init() {
    ETNode<ETGameBoardAppearAnimation>::connect(getEntityId());
    alignSpeedWithScreenSize();
    return true;
}

void GameBoardAppearAnimationLogic::alignSpeedWithScreenSize() {
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);
    fallSpeed *= renderPort.y;
}

void GameBoardAppearAnimationLogic::ET_startBoardAppearing() {
    currDuration = 0;
    moveAllElemsOnTop();
    ETNode<ETGameTimerEvents>::connect(getEntityId());
}

void GameBoardAppearAnimationLogic::moveAllElemsOnTop() {
    ET_SendEventReturn(boardSize, getEntityId(), &ETGameBoard::ET_getBoardSize);

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    Vec3 newPt = tm.pt;

    {
        AABB2Di uiBox(0);
        ET_SendEventReturn(uiBox, getEntityId(), &ETUIBox::ET_getAabb2di);
        newPt.y = static_cast<float>(uiBox.top.y);
    }
    {
        AABB2Di boardBox(0);
        ET_SendEventReturn(boardBox, getEntityId(), &ETGameBoard::ET_getBoardBox);
        Vec2i boxSize = boardBox.getSize();
        newPt.y += static_cast<float>(boxSize.y) / 2.f;
    }

    Vec3 ptShift = newPt - tm.pt;
    for(int i = 0; i < boardSize.x; ++i) {
        for(int j = 0; j < boardSize.y; ++j) {
            EntityId elemEntId;
            ET_SendEventReturn(elemEntId, &ETGameBoard::ET_getElemByBoardPos, Vec2i(i, j));

            Transform elemTm;
            ET_SendEventReturn(elemTm, elemEntId, &ETEntity::ET_getTransform);
            elemTm.pt += ptShift;

            ET_SendEvent(elemEntId, &ETEntity::ET_setTransform, elemTm);
        }
    }
    shift = ptShift.y;
}

void GameBoardAppearAnimationLogic::moveCollumnElemDown(int collumnIdx, float collumnDuration) {
    float elemDelay = 0.01f;
    for(int i = 0; i < boardSize.y; ++i) {
        float elemDuration = collumnDuration - i * elemDelay;
        if(elemDuration < 0.f) {
            break;
        }

        Vec2i boardPt(collumnIdx, i);

        EntityId elemEntId;
        ET_SendEventReturn(elemEntId, &ETGameBoard::ET_getElemByBoardPos, boardPt);

        Transform elemTm;
        ET_SendEventReturn(elemTm, elemEntId, &ETEntity::ET_getTransform);

        Vec3 targetPt(0.f);
        ET_SendEventReturn(targetPt, &ETGameBoard::ET_getPosFromBoardPos, boardPt);

        Vec3 startPt = targetPt;
        startPt.y += shift;

        float prog = std::min(elemDuration / duration, 1.f);
        elemTm.pt = Math::Lerp(startPt, targetPt, prog * prog);

        ET_SendEvent(elemEntId, &ETEntity::ET_setTransform, elemTm);
    }
}

void GameBoardAppearAnimationLogic::ET_onGameTick(float dt) {
    currDuration += dt;
    bool finishLastCollumn = false;
    for(int i = 0; i < boardSize.x; ++i) {
        float collumDuration = currDuration - collumnDelay * i;
        if(collumDuration < 0.f) {
            break;
        }
        moveCollumnElemDown(i, collumDuration);
        if(i == boardSize.x - 1 && collumDuration >= 2.f * duration) {
            finishLastCollumn = true;
        }
    }

    if(finishLastCollumn) {
        ETNode<ETGameTimerEvents>::disconnect();
        ET_SendEvent(&ETGameBoardAppearAnimationEvents::ET_onBoardAppeared);
    }
}
