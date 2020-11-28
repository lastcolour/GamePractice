#include "Game/Logics/LevelButton.hpp"
#include "Reflect/ReflectContext.hpp"
#include "UI/ETUIBox.hpp"
#include "Render/ETRenderNode.hpp"
#include "Core/ETLogger.hpp"
#include "UI/ETUIAnimation.hpp"

void LevelButton::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<LevelButton>("LevelButton")) {
        classInfo->addField("levelIdLabel", &LevelButton::levelIdLabel);
        classInfo->addField("senderId", &LevelButton::senderId);
        classInfo->addField("lockedBackgroundId", &LevelButton::lockedBackgroundId);
        classInfo->addField("unlockedBackgroundId", &LevelButton::unlockedBackgroundId);
        classInfo->addField("completedBackgroundId", &LevelButton::completedBackgroundId);
        classInfo->addField("stars", &LevelButton::stars);
    }
}

LevelButton::LevelButton() :
    state(ELevelButtonState::Locked) {
}

LevelButton::~LevelButton() {
}

bool LevelButton::init() {
    ETNode<ETLevelButton>::connect(getEntityId());
    ETNode<ETUIAnimationSequenceEvent>::connect(getEntityId());
    ET_SendEvent(stars.fristId, &ETUIElement::ET_hide);
    ET_SendEvent(stars.secondId, &ETUIElement::ET_hide);
    ET_SendEvent(stars.thirdId, &ETUIElement::ET_hide);
    ET_setLevelState(ELevelButtonState::Locked);
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

void LevelButton::ET_setLevelState(ELevelButtonState newState) {
    switch(newState) {
        case ELevelButtonState::Locked: {
            ET_SendEvent(senderId, &ETUIElement::ET_disable);
            ET_SendEvent(senderId, &ETUIBox::ET_setRenderId, lockedBackgroundId);
            ET_SendEvent(completedBackgroundId, &ETRenderNode::ET_hide);
            ET_SendEvent(unlockedBackgroundId, &ETRenderNode::ET_hide);
            break;
        }
        case ELevelButtonState::Unlocked: {
            ET_SendEvent(senderId, &ETUIElement::ET_enable);
            ET_SendEvent(senderId, &ETUIBox::ET_setRenderId, unlockedBackgroundId);
            ET_SendEvent(lockedBackgroundId, &ETRenderNode::ET_hide);
            ET_SendEvent(completedBackgroundId, &ETRenderNode::ET_hide);
            break;
        }
        case ELevelButtonState::Completed: {
            ET_SendEvent(senderId, &ETUIElement::ET_enable);
            ET_SendEvent(senderId, &ETUIBox::ET_setRenderId, completedBackgroundId);
            ET_SendEvent(lockedBackgroundId, &ETRenderNode::ET_hide);
            ET_SendEvent(unlockedBackgroundId, &ETRenderNode::ET_hide);
            break;
        }
        default: {
            break;
        }
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

void LevelButton::ET_playChangeAnimation(ELevelButtonState newState, int prevStarCount, int newStarCount) {
    if(newState != state) {
        ET_setLevelState(newState);
        UI::PlayAnimation(getEntityId(), EAnimSequenceType::Highlight, getEntityId());
        ET_SendEvent(senderId, &ETUIElement::ET_disable);
    }
    if(prevStarCount != newStarCount) {
        ET_setLevelStars(newStarCount);
    }
}

void LevelButton::ET_onAnimationPlayed(EntityId sourceId, EAnimSequenceType animType) {
    if(animType != EAnimSequenceType::Highlight) {
        return;
    }
    ET_SendEvent(senderId, &ETUIElement::ET_enable);
}