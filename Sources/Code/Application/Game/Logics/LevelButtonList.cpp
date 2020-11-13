#include "Game/Logics/LevelButtonList.hpp"
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
        classInfo->addField("starsRequired", &LevelButtonItem::startsRequired);
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

bool LevelButtonList::init() {
    ETNode<ETLevelButtonList>::connect(getEntityId());
    initLevelProress();
    return true;
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
    int currStarDone = ET_getDoneStars();
    for(auto& button : levelButtons) {
        if(!button.buttonId.isValid()) {
            LogWarning("[LevelButtonList::init] Invalid button id");
            continue;
        }
        ET_SendEvent(button.buttonId, &ETLevelButton::ET_setLevelId, button.levelId.c_str());
        if(button.startsRequired > currStarDone) {
            ET_SendEvent(button.buttonId, &ETLevelButton::ET_setLevelState, ELevelButtonState::Locked);
            ET_SendEvent(button.buttonId, &ETLevelButton::ET_setLevelStars, 0);
        } else {
            const LevelProgress* lvlProgress = nullptr;
            ET_SendEventReturn(lvlProgress, &ETLevelsProgression::ET_getLevelProgress, button.levelName.c_str());
            int levelStars = 0;
            if(lvlProgress) {
                levelStars = lvlProgress->stars;
            }
            ET_SendEvent(button.buttonId, &ETLevelButton::ET_setLevelStars, levelStars);
            if(levelStars == 3) {
                ET_SendEvent(button.buttonId, &ETLevelButton::ET_setLevelState, ELevelButtonState::Completed);
            } else {
                ET_SendEvent(button.buttonId, &ETLevelButton::ET_setLevelState, ELevelButtonState::Unlocked);
            }
        }
        ET_SendEventReturn(button.senderId, button.buttonId, &ETLevelButton::ET_getSenderId);
    }
}

void LevelButtonList::ET_updateLevelProgress() {
    const LevelProgressDelta* progressDelta = nullptr;
    ET_SendEventReturn(progressDelta, &ETLevelsProgression::ET_getProgressDelta);
    if(!progressDelta) {
        return;
    }
    int currStarsDone = ET_getDoneStars();
    int starsDelta = std::max(0, progressDelta->current.stars - progressDelta->prev.stars);
    if(starsDelta == 0) {
        return;
    }
    EntityId progressedLevelId;
    std::vector<EntityId> newUnlockedLevels;
    for(auto& lvlButton : levelButtons) {
        if(lvlButton.levelName == progressDelta->current.name) {
            progressedLevelId = lvlButton.buttonId;
        }
        if(lvlButton.startsRequired < currStarsDone - starsDelta) {
            continue;
        }
        if(lvlButton.startsRequired >= currStarsDone) {
            continue;
        }
        newUnlockedLevels.push_back(lvlButton.buttonId);
    }

    {
        assert(progressedLevelId.isValid() && "Invalid progressed level");
        auto newState = ELevelButtonState::Unlocked;
        if(progressDelta->current.stars == 3) {
            newState = ELevelButtonState::Completed;
        }
        ET_SendEvent(progressedLevelId, &ETLevelButton::ET_playChangeAnimation,
            newState, progressDelta->prev.stars, progressDelta->current.stars);
    }

    for(auto buttonId : newUnlockedLevels) {
        ET_SendEvent(buttonId, &ETLevelButton::ET_playChangeAnimation,
            ELevelButtonState::Unlocked, 0, 0);
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