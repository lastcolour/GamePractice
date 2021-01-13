#include "Game/Logics/LevelButton.hpp"
#include "Reflect/ReflectContext.hpp"
#include "UI/ETUIBox.hpp"
#include "Render/ETRenderNode.hpp"
#include "Core/ETLogger.hpp"
#include "UI/ETUIAnimation.hpp"
#include "Game/ViewScripts/EventSequence.hpp"

#include <cassert>

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

void LevelButton::init() {
    ETNode<ETLevelButton>::connect(getEntityId());
    ET_SendEvent(stars.fristId, &ETUIElement::ET_hide);
    ET_SendEvent(stars.secondId, &ETUIElement::ET_hide);
    ET_SendEvent(stars.thirdId, &ETUIElement::ET_hide);
    ET_setLevelState(ELevelButtonState::Locked);
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
    state = newState;
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

void LevelButton::ET_scheduleChanges(EventSequence& eventSeq, ELevelButtonState newState, int prevStarCount, int newStarCount) {
    if(prevStarCount != newStarCount) {
        if(prevStarCount == 0 && newStarCount > 0) {
            eventSeq.addEvent(stars.fristId, EAnimSequenceType::Appear);
        }
        if(prevStarCount <= 1 && newStarCount > 1) {
            eventSeq.addEvent(stars.secondId, EAnimSequenceType::Appear);
        }
        if(prevStarCount <= 2 && newStarCount > 2) {
            eventSeq.addEvent(stars.thirdId, EAnimSequenceType::Appear);
        }
    }
    if(newState != state) {
        eventSeq.addEventWithAfterCall(getEntityId(), EAnimSequenceType::Highlight, [newState](EntityId targetId){
            ET_SendEvent(targetId, &ETLevelButton::ET_setLevelState, newState);
            ET_SendEvent(targetId, &ETUIElement::ET_enable);
        });
    }
}