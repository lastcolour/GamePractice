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
    int prevHighScore = 0;
    ET_SendEventReturn(prevHighScore, &ETGameConfig::ET_getHighScore);

    int currScore = 0;
    ET_SendEventReturn(currScore, &ETGameScore::ET_getGameScore);

    const char* extendEntityName = normalExtend.c_str();
    if(currScore > prevHighScore) {
        extendEntityName = newHighScoreExtend.c_str();
    }

    bool extendRes = false;
    ET_SendEventReturn(extendRes, &ETEntityManager::ET_extendEntity, getEntityId(), extendEntityName);
    return extendRes;
}