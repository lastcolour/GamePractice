#include "Game/Logics/GameBoardElemLogic.hpp"

GameBoardElemLogic::GameBoardElemLogic() {
}

GameBoardElemLogic::~GameBoardElemLogic() {
}

bool GameBoardElemLogic::serialize(const JSONNode& node) {
    (void)node;
    return true;
}

bool GameBoardElemLogic::init() {
    return true;
}