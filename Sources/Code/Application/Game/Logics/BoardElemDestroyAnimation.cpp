#include "Game/Logics/BoardElemDestroyAnimation.hpp"
#include "Core/ETLogger.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Audio/ETSound.hpp"
#include "Entity/ETEntity.hpp"
#include "Math/Primitivies.hpp"
#include "Render/ETRenderNode.hpp"

#include <algorithm>

void BoardElemDestroyAnimation::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<BoardElemDestroyAnimation>("BoardElemDestroyAnimation")) {
        classInfo->addField("startDelay", &BoardElemDestroyAnimation::startDelay);
        classInfo->addField("duration", &BoardElemDestroyAnimation::duration);
        classInfo->addResourceField("soundEvent", ResourceType::SoundEvent, &BoardElemDestroyAnimation::setDestroySoundEvent);
    }
}

BoardElemDestroyAnimation::BoardElemDestroyAnimation() :
    startDelay(0.1f),
    duration(0.1f),
    currDuration(-1.f) {
}

BoardElemDestroyAnimation::~BoardElemDestroyAnimation() {
}

void BoardElemDestroyAnimation::init() {
    ETNode<ETBoardElemDetroyAnimation>::connect(getEntityId());
}

void BoardElemDestroyAnimation::deinit() {
    if(currDuration >= 0.f) {
        ET_onGameTick(1024.f);
    }
}

void BoardElemDestroyAnimation::ET_onGameTick(float dt) {
    currDuration += dt;
    if(currDuration < startDelay) {
        return;
    }
    auto animDuration = currDuration - startDelay;
    if(animDuration >= duration) {
        currDuration = -1.f;
        ET_SendEvent(getEntityId(), &ETRenderNode::ET_hide);
        ET_SendEvent(getEntityId(), &ETEntity::ET_setLocalTransform, startTm);
        ET_SendEvent(getEntityId(), &ETBoardElemDetroyAnimationEvents::ET_onDestroyAnimEnded);
        ETNode<ETGameTimerEvents>::disconnect();
    } else {
        float prog = std::min(1.f, animDuration / duration);
        auto scale = Math::Lerp(1.f, 0.f, prog);
        auto resTm = startTm;
        resTm.scale *= scale;
        ET_SendEvent(getEntityId(), &ETEntity::ET_setLocalTransform, resTm);
    }
}

void BoardElemDestroyAnimation::setDestroySoundEvent(const char* eventName) {
    ET_SendEventReturn(destroySound, &ETSoundManager::ET_createEvent, eventName);
}

void BoardElemDestroyAnimation::ET_playDestroy() {
    if(currDuration >= 0.f) {
        LogWarning("[BoardElemDestroyAnimation::ET_playDestroy] Destroy animation already playing");
        return;
    }
    currDuration = 0.f;
    ETNode<ETGameTimerEvents>::connect(getEntityId());
    ET_SendEventReturn(startTm, getEntityId(), &ETEntity::ET_getLocalTransform);
    destroySound.emit();
}