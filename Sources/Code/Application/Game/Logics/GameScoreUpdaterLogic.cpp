#include "Game/Logics/GameScoreUpdaterLogic.hpp"
#include "UI/UIETInterfaces.hpp"
#include "Core/StringFormat.hpp"

GameScoreUpdaterLogic::GameScoreUpdaterLogic() {
}

GameScoreUpdaterLogic::~GameScoreUpdaterLogic() {
}

bool GameScoreUpdaterLogic::serialize(const JSONNode& node) {
    return true;
}

bool GameScoreUpdaterLogic::init() {
    ETNode<ETGameScoreUpdater>::connect(getEntityId());
    return true;
}

void GameScoreUpdaterLogic::ET_setGameScore(int score) {
    std::string text = StringFormat("%d", score);
    ET_SendEvent(getEntityId(), &ETUILabel::ET_setText, text.c_str());
}