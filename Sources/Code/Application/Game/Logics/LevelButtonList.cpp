#include "Game/Logics/LevelButtonList.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Core/ETLogger.hpp"

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

bool LevelButtonList::init() {
    ETNode<ETLevelButtonList>::connect(getEntityId());
    for(auto& button : levelButtons) {
        if(!button.buttonId.isValid()) {
            LogWarning("[LevelButtonList::init] Invalid button id");
            continue;
        }
        ET_SendEvent(button.buttonId, &ETLevelButton::ET_setLevelId, button.levelId.c_str());
        ET_SendEvent(button.buttonId, &ETLevelButton::ET_setLevelLocked, false);
        ET_SendEvent(button.buttonId, &ETLevelButton::ET_setLevelStars, 0);

        ET_SendEventReturn(button.senderId, button.buttonId, &ETLevelButton::ET_getSenderId);
    }
    return true;
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