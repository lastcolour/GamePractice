#include "Game/Progression/LevelButtonList.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Core/ETLogger.hpp"
#include "Game/Progression/LevelsProgressData.hpp"
#include "UI/ETUIAnimation.hpp"

#include <algorithm>
#include <cassert>

void LevelButtonItem::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<LevelButtonItem>("LevelButtonItem")) {
        classInfo->addField("id", &LevelButtonItem::levelId);
        classInfo->addResourceField("levelName", ResourceType::Entity, &LevelButtonItem::levelName);
        classInfo->addField("buttonId", &LevelButtonItem::buttonId);
    }
}

void LevelButtonList::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<LevelButtonList>("LevelButtonList")) {
        classInfo->addField("buttons", &LevelButtonList::levelButtons);
    }
}

LevelButtonList::LevelButtonList() {
}

LevelButtonList::~LevelButtonList() {
}

void LevelButtonList::init() {
    ETNode<ETLevelButtonList>::connect(getEntityId());
    initLevelProress();
}

void LevelButtonList::deinit() {
}

int LevelButtonList::ET_getTotalStars() const {
    return levelButtons.size() * 3;
}

int LevelButtonList::ET_getDoneStars() const {
    int currStarDone = 0;
    ET_SendEventReturn(currStarDone, &ETLevelsProgression::ET_getStarsDone);
    return currStarDone;
}

void LevelButtonList::initLevelProress() {
    bool prevLevelHasStars = true;
    for(auto& button : levelButtons) {
        if(!button.buttonId.isValid()) {
            LogWarning("[LevelButtonList::init] Invalid button id");
            continue;
        }
        ET_SendEventReturn(button.senderId, button.buttonId, &ETLevelButton::ET_getSenderId);
        ET_SendEvent(button.buttonId, &ETLevelButton::ET_setLevelId, button.levelId.c_str());

        const LevelProgress* lvlProgress = nullptr;
        ET_SendEventReturn(lvlProgress, &ETLevelsProgression::ET_getLevelProgress, button.levelName.c_str());
        if(lvlProgress) {
            assert(lvlProgress->stars > 0 && "Invalid stars amount");
            prevLevelHasStars = true;
            ET_SendEvent(button.buttonId, &ETLevelButton::ET_setLevelState, ELevelButtonState::Unlocked, lvlProgress->stars);
        } else {
            if(prevLevelHasStars) {
                ET_SendEvent(button.buttonId, &ETLevelButton::ET_setLevelState, ELevelButtonState::Unlocked, 0);
            } else {
                ET_SendEvent(button.buttonId, &ETLevelButton::ET_setLevelState, ELevelButtonState::Locked, 0);
            }
            prevLevelHasStars = false;
        }
    }
}

void LevelButtonList::ET_updateLevelProgress(EventSequence& eventSeq) {
    const LevelProgressDelta* progressDelta = nullptr;
    ET_SendEventReturn(progressDelta, &ETLevelsProgression::ET_getProgressDelta);
    if(!progressDelta) {
        return;
    }
    int starsDelta = std::max(0, progressDelta->current.stars - progressDelta->prev.stars);
    if(starsDelta == 0) {
        return;
    }

    EntityId progressedLevelId;
    EntityId newUnlockedLevelId;

    bool nextLevelUnlocked = false;
    for(auto& lvlButton : levelButtons) {
        if(lvlButton.levelName == progressDelta->current.name) {
            progressedLevelId = lvlButton.buttonId;
            nextLevelUnlocked = true;
            continue;
        }
        if(nextLevelUnlocked) {
            newUnlockedLevelId = lvlButton.buttonId;
            break;
        }
    }

    assert(progressedLevelId.isValid() && "Invalid progressed level");
    ET_SendEvent(progressedLevelId, &ETLevelButton::ET_scheduleChanges, eventSeq,
        ELevelButtonState::Unlocked, progressDelta->current.stars);

    if(newUnlockedLevelId.isValid()) {
        ET_SendEvent(newUnlockedLevelId, &ETLevelButton::ET_scheduleChanges, eventSeq,
            ELevelButtonState::Unlocked, 0);
    }
}

const char* LevelButtonList::ET_getLevelNameForSender(EntityId senderId) const {
    for(auto& button : levelButtons) {
        if(button.senderId == senderId) {
            return button.levelName.c_str();
        }
    }
    return "";
}

EntityId LevelButtonList::ET_getLastUnlockedLevelButton() const {
    EntityId lastUnlockedBtId;
    for(auto& button : levelButtons) {
        ELevelButtonState btState = ELevelButtonState::Locked;
        ET_SendEventReturn(btState, button.buttonId, &ETLevelButton::ET_getState);
        if(btState == ELevelButtonState::Unlocked) {
            lastUnlockedBtId = button.buttonId;
        }
    }
    return lastUnlockedBtId;
}