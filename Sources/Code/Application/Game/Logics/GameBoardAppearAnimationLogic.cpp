#include "Game/Logics/GameBoardAppearAnimationLogic.hpp"
#include "Entity/ETEntityInterfaces.hpp"
#include "UI/ETUIInterfaces.hpp"

GameBoardAppearAnimationLogic::GameBoardAppearAnimationLogic() :
    boardSize(0) {
}

GameBoardAppearAnimationLogic::~GameBoardAppearAnimationLogic() {
}

bool GameBoardAppearAnimationLogic::serialize(const JSONNode& node) {
    (void)node;
    return true;
}

bool GameBoardAppearAnimationLogic::init() {
    ETNode<ETGameBoardAppearAnimation>::connect(getEntityId());
    return true;
}

void GameBoardAppearAnimationLogic::ET_startBoardAppearing() {
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
        newPt.y = static_cast<float>(uiBox.bot.y);
    }
    {
        AABB2Di boardBox(0);
        ET_SendEventReturn(boardBox, getEntityId(), &ETGameBoard::ET_getBoardBox);
        Vec2i boxSize = boardBox.getSize();
        newPt.y += static_cast<float>(boxSize.y) / 2.f;
    }

    Vec3 ptShift = tm.pt - newPt;
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
}

void GameBoardAppearAnimationLogic::moveCollumnElemDown(int collumnIdx) {
    for(int i = 0; i < boardSize.y; ++i) {
        EntityId elemEntId;
        ET_SendEventReturn(elemEntId, &ETGameBoard::ET_getElemByBoardPos, Vec2i(collumnIdx, i));

        Transform elemTm;
        ET_SendEventReturn(elemTm, elemEntId, &ETEntity::ET_getTransform);

        elemTm.pt.y -= 0.1f;

        ET_SendEvent(elemEntId, &ETEntity::ET_setTransform, elemTm);
    }
}

void GameBoardAppearAnimationLogic::ET_onGameTick(float dt) {
    (void)dt;
    for(int i = 0; i < boardSize.x; ++i) {
        moveCollumnElemDown(i);
    }
}
