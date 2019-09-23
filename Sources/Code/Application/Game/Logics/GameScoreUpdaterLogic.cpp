#include "Game/Logics/GameScoreUpdaterLogic.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "Core/StringFormat.hpp"
#include "Core/JSONNode.hpp"

#include <algorithm>

GameScoreUpdaterLogic::GameScoreUpdaterLogic() :
    currentStepDelay(0.f),
    increaseSpeed(10),
    currentValue(0),
    targetValue(0) {
}

GameScoreUpdaterLogic::~GameScoreUpdaterLogic() {
}

bool GameScoreUpdaterLogic::serialize(const JSONNode& node) {
    node.read("stepIncrease", increaseSpeed);
    return true;
}

bool GameScoreUpdaterLogic::init() {
    ETNode<ETGameScoreUpdater>::connect(getEntityId());
    ETNode<ETGameTimerEvents>::connect(getEntityId());
    return true;
}

void GameScoreUpdaterLogic::ET_setGameScore(int score) {
    targetValue = score;
}

void GameScoreUpdaterLogic::ET_onGameTick(float dt) {
    if(currentValue == targetValue) {
        return;
    }

    currentStepDelay += dt;
    int increaseStep = static_cast<int>((static_cast<float>(increaseSpeed) * currentStepDelay));
    if(increaseStep == 0) {
        return;
    }

    currentStepDelay = 0.f;
    currentValue = std::min(currentValue + increaseStep, targetValue);

    std::string text = StringFormat("%d", currentValue);
    ET_SendEvent(getEntityId(), &ETUILabel::ET_setText, text.c_str());
}