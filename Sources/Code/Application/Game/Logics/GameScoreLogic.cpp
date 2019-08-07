#include "Game/Logics/GameScoreLogic.hpp"

GameScoreLogic::GameScoreLogic() :
    score(0) {
}

GameScoreLogic::~GameScoreLogic() {
}

bool GameScoreLogic::serialize(const JSONNode& node) {
    return true;
}

bool GameScoreLogic::init() {
    ETNode<ETGameScore>::connect(getEntityId());
    return true;
}

void GameScoreLogic::ET_onElemsDestroyed(int count) {
    score += count;
    ET_SendEvent(&ETGameScoreUpdater::ET_setGameScore, score);
}