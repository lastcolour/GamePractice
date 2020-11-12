#include "Game/Logics/LevelButton.hpp"
#include "Reflect/ReflectContext.hpp"
#include "UI/ETUIBox.hpp"
#include "Render/ETRenderNode.hpp"
#include "Core/ETLogger.hpp"

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
    ET_SendEvent(stars.fristId, &ETUIElement::ET_hide);
    ET_SendEvent(stars.secondId, &ETUIElement::ET_hide);
    ET_SendEvent(stars.thirdId, &ETUIElement::ET_hide);
    ET_setLevelLocked(true);
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
        ET_SendEvent(senderId, &ETUIElement::ET_disable);
        ET_SendEvent(senderId, &ETUIBox::ET_setRenderId, lockedBackgroundId);
        ET_SendEvent(completedBackgroundId, &ETRenderNode::ET_hide);
    } else {
        ET_SendEvent(senderId, &ETUIElement::ET_enable);
        ET_SendEvent(senderId, &ETUIBox::ET_setRenderId, completedBackgroundId);
        ET_SendEvent(lockedBackgroundId, &ETRenderNode::ET_hide);
    }
}

void LevelButton::ET_setLevelStars(int count) {
    if(count == 0) {
        ET_SendEvent(stars.fristId, &ETUIElement::ET_hide);
        ET_SendEvent(stars.secondId, &ETUIElement::ET_hide);
        ET_SendEvent(stars.thirdId, &ETUIElement::ET_hide);
    } else if(count == 1) {
        ET_SendEvent(stars.fristId, &ETUIElement::ET_show);
        ET_SendEvent(stars.secondId, &ETUIElement::ET_hide);
        ET_SendEvent(stars.thirdId, &ETUIElement::ET_hide);
    } else if(count == 2) {
        ET_SendEvent(stars.fristId, &ETUIElement::ET_show);
        ET_SendEvent(stars.secondId, &ETUIElement::ET_show);
        ET_SendEvent(stars.thirdId, &ETUIElement::ET_hide);
    } else if(count == 3) {
        ET_SendEvent(stars.fristId, &ETUIElement::ET_show);
        ET_SendEvent(stars.secondId, &ETUIElement::ET_show);
        ET_SendEvent(stars.thirdId, &ETUIElement::ET_show);
    } else {
        LogError("[LevelButton::ET_setLevelStars] Invalid amount of stars: %d", count);
    }
}