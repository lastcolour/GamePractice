#include "UI/Logics/UIResultInfoSelector.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Entity/ETEntityInterfaces.hpp"
#include "Core/JSONNode.hpp"

UIResultInfoSelector::UIResultInfoSelector() {
}

UIResultInfoSelector::~UIResultInfoSelector() {
}

bool UIResultInfoSelector::serialize(const JSONNode& node) {
    node.read("normalScore", normalExtend);
    node.read("newHighScore", newHighScoreExtend);
    return true;
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