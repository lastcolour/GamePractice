#include "UI/Logics/UIResultInfoSelector.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Entity/ETEntityInterfaces.hpp"

UIResultInfoSelector::UIResultInfoSelector() {
}

UIResultInfoSelector::~UIResultInfoSelector() {
}

void UIResultInfoSelector::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIResultInfoSelector>("UIResultInfoSelector")) {
        classInfo->addField("normalSocre", &UIResultInfoSelector::normalExtend);
        classInfo->addField("newHighScore", &UIResultInfoSelector::newHighScoreExtend);
    }
}

bool UIResultInfoSelector::init() {
    const EndGameResult* endResult = nullptr;
    ET_SendEventReturn(endResult, &ETGameEndResult::ET_getGameEndResult);
    const char* extendEntityName = normalExtend.c_str();
    if(endResult && endResult->newHighScore) {
        extendEntityName = newHighScoreExtend.c_str();
    }
    bool extendRes = false;
    ET_SendEventReturn(extendRes, &ETEntityManager::ET_extendEntity, getEntityId(), extendEntityName);
    return extendRes;
}

void UIResultInfoSelector::deinit() {
}