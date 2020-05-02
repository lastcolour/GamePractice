#include "Game/Logics/GameBoardElemLogic.hpp"

GameBoardElemLogic::GameBoardElemLogic() {
}

GameBoardElemLogic::~GameBoardElemLogic() {
}

void GameBoardElemLogic::Reflect(ReflectContext& ctx) {
    ctx.classInfo<GameBoardElemLogic>("GameBoardElem");
}

bool GameBoardElemLogic::init() {
    return true;
}

void GameBoardElemLogic::deinit() {
}
