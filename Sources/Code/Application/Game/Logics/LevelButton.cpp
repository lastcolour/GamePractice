#include "Game/Logics/LevelButton.hpp"
#include "Reflect/ReflectContext.hpp"
#include "UI/ETUIBox.hpp"

void LevelButton::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<LevelButton>("LevelButton")) {
        classInfo->addField("levelIdLabel", &LevelButton::levelIdLabel);
        classInfo->addField("senderId", &LevelButton::senderId);
        classInfo->addField("lockedBackgroundId", &LevelButton::lockedBackgroundId);
        classInfo->addField("completedBackgroundId", &LevelButton::completedBackgroundId);
        classInfo->addField("stars", &LevelButton::stars);
    }
}

LevelButton::LevelButton() {
}

LevelButton::~LevelButton() {
}

bool LevelButton::init() {
    ETNode<ETLevelButton>::connect(getEntityId());
    return true;
}

void LevelButton::deinit() {
}

EntityId LevelButton::ET_getSenderId() const {
    return senderId;
}

void LevelButton::ET_setLevelId(const char* levelId) {
    ET_SendEvent(levelIdLabel, &ETUILabel::ET_setText, levelId);
}

void LevelButton::ET_setLevelLocked(bool flag) {
    if(flag) {

    } else {

    }
}

void LevelButton::ET_setLevelStars(int count) {
}