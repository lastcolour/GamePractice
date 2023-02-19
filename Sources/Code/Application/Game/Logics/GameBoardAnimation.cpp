#include "Game/Logics/GameBoardAnimation.hpp"
#include "UI/ETUIBox.hpp"

void GameBoardAnimation::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardAnimation>("GameBoardAnimation")) {
        classInfo->addField("minScale", &GameBoardAnimation::minScale);
        classInfo->addField("scaleDuration", &GameBoardAnimation::scaleDuration);
    }
}

GameBoardAnimation::GameBoardAnimation() :
    scaleDuration(0.3f),
    minScale(0.8f),
    currDuration(0.f) {
}

GameBoardAnimation::~GameBoardAnimation() {
}

void GameBoardAnimation::init() {
    ETNode<ETGameBoardAnimation>::connect(getEntityId());
    ETNode<ETGameBoardSpawnerEvents>::connect(getEntityId());
}

void GameBoardAnimation::deinit() {
}

void GameBoardAnimation::ET_onGameTick(float dt) {
    currDuration += dt;
    float prog = std::min(currDuration / scaleDuration, 1.f);
    float currScale = Math::Lerp(1.f, minScale, prog);

    if(currDuration >= scaleDuration) {
        ET_SendEvent(&ETGameBoardAnimationEvents::ET_onZoomOutPlayed);
        ETNode<ETGameTimerEvents>::disconnect();
    }

    Transform tm = getLocalTransform();

    tm.scale = Vec3(currScale);
    ET_SendEvent(getEntityId(), &ETUIElement::ET_setIgnoreTransform, true);
    setLocalTransform(tm);
    ET_SendEvent(getEntityId(), &ETUIElement::ET_setIgnoreTransform, false);
}

void GameBoardAnimation::ET_zoomOut() {
    currDuration = 0.f;
    ETNode<ETGameTimerEvents>::connect(getEntityId());
}

void GameBoardAnimation::ET_onStartLoading() {
    Transform tm = getLocalTransform();
    tm.scale = Vec3(1.f);
    setLocalTransform(tm);
}

void GameBoardAnimation::ET_onStartDestroying() {
    ET_onStartLoading();
}