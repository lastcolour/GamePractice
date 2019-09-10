#include "Game/Logics/GameScoreLogic.hpp"

GameScoreLogic::GameScoreLogic() :
    score(0) {
}

GameScoreLogic::~GameScoreLogic() {
}

bool GameScoreLogic::serialize(const JSONNode& node) {
    (void)node;
    return true;
}

bool GameScoreLogic::init() {
    ETNode<ETGameScore>::connect(getEntityId());
    ETNode<ETGameBoardElemDestoryEvents>::connect(getEntityId());
    return true;
}

void GameScoreLogic::ET_onElemsDestroyed(int count) {
    score += count;
    ET_SendEvent(&ETGameScoreUpdater::ET_setGameScore, score);
}

int GameScoreLogic::ET_getGameScore() const {
    return score;
}