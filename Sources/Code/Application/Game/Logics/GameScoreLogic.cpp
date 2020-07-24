#include "Game/Logics/GameScoreLogic.hpp"
#include "Reflect/ReflectContext.hpp"

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
    score = 0;
}

void GameScoreLogic::ET_resetScore() {
    score = 0;
    ET_SendEvent(&ETGameScoreUpdater::ET_setGameScore, 0);
}

void GameScoreLogic::ET_onElemsDestroyed(EntityId elemId) {
    score += 1;
    ET_SendEvent(&ETGameScoreUpdater::ET_setGameScore, score);
}

int GameScoreLogic::ET_getGameScore() const {
    return score;
}