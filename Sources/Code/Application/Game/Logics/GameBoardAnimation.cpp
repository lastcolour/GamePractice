#include "Game/Logics/GameBoardAnimation.hpp"
#include "Entity/ETEntity.hpp"
#include "Reflect/ReflectContext.hpp"

#include <algorithm>

void GameBoardAnimation::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardAnimation>("GameBoardAnimation")) {
        classInfo->addField("minScale", &GameBoardAnimation::minScale);
        classInfo->addField("scaleDuration", &GameBoardAnimation::scaleDuration);
    }
}

GameBoardAnimation::GameBoardAnimation() :
    startScale(1.f),
    scaleDuration(0.3f),
    minScale(0.8f),
    currDuration(0.f) {
}

GameBoardAnimation::~GameBoardAnimation() {
}

bool GameBoardAnimation::init() {
    ETNode<ETGameBoardAnimation>::connect(getEntityId());
    return true;
}

void GameBoardAnimation::deinit() {
}

void GameBoardAnimation::ET_onGameTick(float dt) {

    currDuration += dt;
    float prog = std::min(currDuration / scaleDuration, 1.f);
    float currScale = Math::Lerp(1.f, minScale, prog);

    if(currDuration >= scaleDuration) {
        ETNode<ETGameTimerEvents>::disconnect();
    }

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);

    tm.scale = startScale * currScale;
    ET_SendEvent(getEntityId(), &ETEntity::ET_setTransform, tm);
}

void GameBoardAnimation::ET_zoomOut() {
    currDuration = 0.f;

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    startScale = tm.scale;

    ETNode<ETGameTimerEvents>::connect(getEntityId());
}