#include "Game/Logics/GameScoreUpdaterLogic.hpp"
#include "UI/ETUIBox.hpp"
#include "Core/StringFormat.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Audio/ETSound.hpp"

#include <algorithm>

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
        classInfo->addResourceField("increseSound", ResourceType::SoundEvent, &GameScoreUpdaterLogic::setIncreaseSoundEvent);
    }
}

bool GameScoreUpdaterLogic::init() {
    ETNode<ETGameScoreUpdater>::connect(getEntityId());
    ETNode<ETGameTimerEvents>::connect(getEntityId());
    ET_SendEvent(getEntityId(), &ETUILabel::ET_setText, "0");
    return true;
}

void GameScoreUpdaterLogic::deinit() {
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
    ET_SendEvent(getEntityId(), &ETUILabel::ET_setText, text.c_str());
}

void GameScoreUpdaterLogic::setIncreaseSoundEvent(const char* soundName) {
    ET_SendEventReturn(increaseSound, &ETSoundManager::ET_createEvent, soundName);
}