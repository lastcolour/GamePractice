#include "Game/Logics/GameScoreUpdaterLogic.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/ETUIProgressBar.hpp"
#include "Game/ETGameScore.hpp"

GameScoreUpdaterLogic::GameScoreUpdaterLogic() :
    currentStepDelay(0.f),
    increaseSpeed(10),
    currentValue(0),
    targetValue(0) {
}

GameScoreUpdaterLogic::~GameScoreUpdaterLogic() {
}

void GameScoreUpdaterLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameScoreUpdaterLogic>("GameScoreUpdater")) {
        classInfo->addField("increaseSpeed", &GameScoreUpdaterLogic::increaseSpeed);
        classInfo->addField("amountLabelId", &GameScoreUpdaterLogic::amountLabelId);
        classInfo->addField("progressBardId", &GameScoreUpdaterLogic::progressBardId);
        classInfo->addField("increseSound", &GameScoreUpdaterLogic::increaseSound);
    }
}

void GameScoreUpdaterLogic::init() {
    ETNode<ETGameScoreUpdater>::connect(getEntityId());
    ET_reset();
    ET_resume();
}

void GameScoreUpdaterLogic::deinit() {
}

void GameScoreUpdaterLogic::ET_reset() {
    currentValue = 0;
    targetValue = 0;
    ET_SendEvent(amountLabelId, &ETUILabel::ET_setText, "0");
    ET_SendEvent(progressBardId, &ETUIProgressBar::ET_setProgress, 0.f);
}

void GameScoreUpdaterLogic::ET_pause() {
    ETNode<ETGameTimerEvents>::disconnect();
}

void GameScoreUpdaterLogic::ET_resume() {
    ETNode<ETGameTimerEvents>::connect(getEntityId());
}

void GameScoreUpdaterLogic::ET_setGameScore(int score) {
    targetValue = score;
}

void GameScoreUpdaterLogic::ET_onGameTick(float dt) {
    if(currentValue == targetValue) {
        return;
    }

    if(targetValue > currentValue) {
        increaseSound.emit();
    }

    currentStepDelay += dt;
    int increaseStep = static_cast<int>((static_cast<float>(increaseSpeed) * currentStepDelay));
    if(increaseStep == 0) {
        return;
    }

    currentStepDelay = 0.f;
    currentValue = std::min(currentValue + increaseStep, targetValue);

    std::string text = StringFormat("%d", currentValue);
    ET_SendEvent(amountLabelId, &ETUILabel::ET_setText, text.c_str());

    int threeStarsScore = 0;
    ET_SendEventReturn(threeStarsScore, &ETGameScore::ET_getMixScoreToAchieve, ObjectiveProgress::ThreeStars);
    float prog = 0.f;

    if(threeStarsScore > 0) {
        prog = currentValue / static_cast<float>(threeStarsScore);
    }
    ET_SendEvent(progressBardId, &ETUIProgressBar::ET_setProgress, prog);
}