#include "Game/Progression/LevelButton.hpp"
#include "UI/ETUIBox.hpp"
#include "Render/ETRenderNode.hpp"
#include "UI/ETUIAnimation.hpp"
#include "Game/ViewScripts/EventSequence.hpp"

#include <cassert>

namespace {

void setCompletedBackground(UnlockedLevelButtonData& unlockedBt, bool flag) {
    if(flag) {
        ET_SendEvent(unlockedBt.buttonId, &ETUIBox::ET_setRenderId, unlockedBt.completedRenderId);
        ET_SendEvent(unlockedBt.unlockedRenderId, &ETRenderNode::ET_hide);
    } else {
        ET_SendEvent(unlockedBt.buttonId, &ETUIBox::ET_setRenderId, unlockedBt.unlockedRenderId);
        ET_SendEvent(unlockedBt.completedRenderId, &ETRenderNode::ET_hide);
    }
}

const float HIGHLIGHT_DELAY = 0.2f;

} // namespace

void UnlockedLevelButtonData::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UnlockedLevelButtonData>("UnlockedLevelButtonData")) {
        classInfo->addField("labelId", &UnlockedLevelButtonData::labelId);
        classInfo->addField("buttonId", &UnlockedLevelButtonData::buttonId);
        classInfo->addField("unlockedRenderId", &UnlockedLevelButtonData::unlockedRenderId);
        classInfo->addField("completedRenderId", &UnlockedLevelButtonData::completedRenderId);
        classInfo->addField("stars", &UnlockedLevelButtonData::stars);
    }
}

void LockedLevelButtonData::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<LockedLevelButtonData>("LockedLevelButtonData")) {
        classInfo->addField("labelId", &LockedLevelButtonData::labelId);
        classInfo->addField("buttonId", &LockedLevelButtonData::buttonId);
    }
}

void LevelButton::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<LevelButton>("LevelButton")) {
        classInfo->addField("lockedButton", &LevelButton::lockedBt);
        classInfo->addField("unlockedButton", &LevelButton::unlockedBt);
        classInfo->addField("unlockSound", &LevelButton::unlockSound);
    }
}

LevelButton::LevelButton() :
    state(ELevelButtonState::Locked),
    starsCount(0) {
}

LevelButton::~LevelButton() {
}

void LevelButton::init() {
    ETNode<ETLevelButton>::connect(getEntityId());
    ET_setLevelState(ELevelButtonState::Locked, 0);
}

void LevelButton::deinit() {
}

EntityId LevelButton::ET_getSenderId() const {
    return unlockedBt.buttonId;
}

void LevelButton::ET_setLevelId(const char* levelId) {
    ET_SendEvent(lockedBt.labelId, &ETUILabel::ET_setText, levelId);
    ET_SendEvent(unlockedBt.labelId, &ETUILabel::ET_setText, levelId);
}

ELevelButtonState LevelButton::ET_getState() const {
    return state;
}

void LevelButton::ET_setLevelState(ELevelButtonState newState, int newStarsCount) {
    state = newState;
    starsCount = newStarsCount;
    switch(newState) {
        case ELevelButtonState::Locked: {
            assert(newStarsCount == 0 && "Invalid amout of stars for locked level");
            ET_SendEvent(lockedBt.buttonId, &ETUIElement::ET_show);
            ET_SendEvent(unlockedBt.buttonId, &ETUIElement::ET_hide);
            ET_SendEvent(unlockedBt.unlockedRenderId, &ETRenderNode::ET_hide);
            ET_SendEvent(unlockedBt.completedRenderId, &ETRenderNode::ET_hide);
            break;
        }
        case ELevelButtonState::Unlocked: {
            ET_SendEvent(lockedBt.buttonId, &ETUIElement::ET_hide);
            ET_SendEvent(unlockedBt.buttonId, &ETUIElement::ET_show);
            if(newStarsCount > 0) {
                ET_SendEvent(unlockedBt.stars.fristId, &ETUIElement::ET_show);
            }
            if(newStarsCount > 1) {
                ET_SendEvent(unlockedBt.stars.secondId, &ETUIElement::ET_show);
            }
            if(newStarsCount > 2) {
                ET_SendEvent(unlockedBt.stars.thirdId, &ETUIElement::ET_show);
            }
            if(newStarsCount == 3) {
                setCompletedBackground(unlockedBt, true);
            } else {
                setCompletedBackground(unlockedBt, false);
            }

            break;
        }
        default: {
            assert(false && "Invalid button state");
        }
    }
}

void LevelButton::ET_scheduleChanges(EventSequence& eventSeq, ELevelButtonState newState, int newStarCount) {
    bool playHighlight = false;
    if(state == ELevelButtonState::Locked) {
        if(newState != ELevelButtonState::Unlocked) {
            return;
        }
        playHighlight = true;
        state = newState;
        {
            EventSequence::Event event;
            event.animType = EAnimSequenceType::Disappear;
            event.targetId = lockedBt.buttonId;
            event.onEndCallback = [this](){
                ET_SendEvent(unlockedBt.buttonId, &ETUIElement::ET_show);
                setCompletedBackground(unlockedBt, false);
            };
            eventSeq.addEvent(event);
        }
    }
    if(state == ELevelButtonState::Unlocked) {
        if(newStarCount > 0 && starsCount <= 0) {
            EventSequence::Event event;
            event.animType = EAnimSequenceType::Appear;
            event.targetId = unlockedBt.stars.fristId;
            eventSeq.addEvent(event);
        }
        if(newStarCount > 1 && starsCount <= 1) {
            EventSequence::Event event;
            event.animType = EAnimSequenceType::Appear;
            event.targetId = unlockedBt.stars.secondId;
            eventSeq.addEvent(event);
        }
        if(newStarCount > 2 && starsCount <= 2) {
            playHighlight = true;
            EventSequence::Event event;
            event.animType = EAnimSequenceType::Appear;
            event.targetId = unlockedBt.stars.thirdId;
            event.onEndCallback = [this](){
                setCompletedBackground(unlockedBt, true);
            };
            eventSeq.addEvent(event);
        }
    }
    starsCount = newStarCount;
    if(playHighlight) {
        EventSequence::Event event;
        event.startDelay = HIGHLIGHT_DELAY;
        event.onStartCallback = [this](){
            unlockSound.emit();
        };
        event.animType = EAnimSequenceType::Highlight;
        event.targetId = unlockedBt.buttonId;
        eventSeq.addEvent(event);
    }
}

int LevelButton::ET_getLevelStars() const {
    assert(starsCount >= 0 && "Invalid stars amount");
    return starsCount;
}