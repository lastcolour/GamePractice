#include "Game/Logics/GameScoreLogic.hpp"

GameScoreLogic::GameScoreLogic() :
    score(0) {
}

GameScoreLogic::~GameScoreLogic() {
}

void GameScoreLogic::Reflect(ReflectContext& ctx) {
    ctx.classInfo<GameScoreLogic>("GameScore");
}

bool GameScoreLogic::init() {
    ETNode<ETGameScore>::connect(getEntityId());
    ETNode<ETGameBoardElemDestoryEvents>::connect(getEntityId());
    return true;
}

void GameScoreLogic::deinit() {
}

void GameScoreLogic::ET_onElemsDestroyed(int count) {
    score += count;
    ET_SendEvent(&ETGameScoreUpdater::ET_setGameScore, score);
}

int GameScoreLogic::ET_getGameScore() const {
    return score;
}