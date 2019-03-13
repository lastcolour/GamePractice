#include "Game/Logics/GameBoardElemLogic.hpp"

GameBoardElemLogic::GameBoardElemLogic() :
    boardPos(-1) {
}

GameBoardElemLogic::~GameBoardElemLogic() {
}

bool GameBoardElemLogic::serialize(const JSONNode& node) {
    return true;
}

bool GameBoardElemLogic::init() {
    ETNode<ETGameBoardElemLogic>::connect(getEntityId());
    return true;
}

void GameBoardElemLogic::ET_setBoardPos(const Vec2i& pt) {
    boardPos = pt;
}

const Vec2i& GameBoardElemLogic::ET_getBoardPos() const {
    return boardPos;
}