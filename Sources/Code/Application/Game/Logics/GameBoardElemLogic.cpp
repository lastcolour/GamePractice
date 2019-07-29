#include "Game/Logics/GameBoardElemLogic.hpp"

GameBoardElemLogic::GameBoardElemLogic() :
    boardPos(-1) {
}

GameBoardElemLogic::~GameBoardElemLogic() {
}

bool GameBoardElemLogic::serialize(const JSONNode& node) {
    (void)node;
    return true;
}

bool GameBoardElemLogic::init() {
    ETNode<ETGameBoardElemLogic>::connect(getEntityId());
    ETNode<ETUIInteractionBox>::connect(getEntityId());
    return true;
}

void GameBoardElemLogic::ET_setBoardPos(const Vec2i& pt) {
    boardPos = pt;
}

const Vec2i& GameBoardElemLogic::ET_getBoardPos() const {
    return boardPos;
}

void GameBoardElemLogic::ET_onPress() {
}

void GameBoardElemLogic::ET_onHover(bool flag) {
    (void)flag;
}

bool GameBoardElemLogic::ET_isHovered() const {
    return false;
}

AABB2Di GameBoardElemLogic::ET_getHitBox() const {
    AABB2Di box(0);
    ET_SendEventReturn(box, getEntityId(), &ETUIBox::ET_getAabb2di);
    return box;
}