#include "Game/Logics/LevelButtonList.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Core/ETLogger.hpp"
#include "Game/Progression/LevelsProgressData.hpp"

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
    ET_updateLevelProgress();
    return true;
}

int LevelButtonList::ET_getTotalStars() const {
    return levelButtons.size() * 3;
}

int LevelButtonList::ET_getDoneStars() const {
    int currStarDone = 0;
    ET_SendEventReturn(currStarDone, &ETLevelsProgression::ET_getStarsDone);
    return currStarDone;
}

void LevelButtonList::ET_updateLevelProgress() {
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

void LevelButtonList::deinit() {
}

const char* LevelButtonList::ET_getLevelNameForSender(EntityId senderId) const {
    for(auto& button : levelButtons) {
        if(button.senderId == senderId) {
            return button.levelName.c_str();
        }
    }
    return "";
}