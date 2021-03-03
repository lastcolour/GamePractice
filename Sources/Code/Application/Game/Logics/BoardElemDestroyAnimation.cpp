#include "Game/Logics/BoardElemDestroyAnimation.hpp"
#include "Core/ETLogger.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Audio/ETSound.hpp"
#include "Entity/ETEntity.hpp"
#include "Math/Primitivies.hpp"
#include "Render/ETRenderNode.hpp"

#include <algorithm>
#include <cassert>

void BoardElemDestroyAnimation::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<BoardElemDestroyAnimation>("BoardElemDestroyAnimation")) {
        classInfo->addField("startDelay", &BoardElemDestroyAnimation::startDelay);
        classInfo->addField("duration", &BoardElemDestroyAnimation::duration);
        classInfo->addField("endDelay", &BoardElemDestroyAnimation::endDelay);
        classInfo->addResourceField("soundEvent", ResourceType::SoundEvent,
            &BoardElemDestroyAnimation::setDestroySoundEvent);
    }
}

BoardElemDestroyAnimation::BoardElemDestroyAnimation() :
    startDelay(0.1f),
    duration(0.1f),
    endDelay(0.1f),
    currDuration(-1.f),
    currState(State::Ended) {
}

BoardElemDestroyAnimation::~BoardElemDestroyAnimation() {
}

void BoardElemDestroyAnimation::init() {
    ETNode<ETBoardElemDestroyAnimation>::connect(getEntityId());
}

void BoardElemDestroyAnimation::deinit() {
    if(currState != State::Ended) {
        ET_onGameTick(1024.f);
    }
}

void BoardElemDestroyAnimation::ET_onGameTick(float dt) {

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getLocalTransform);

    currDuration += dt;
    switch(currState) {
        case State::Starting: {
            if(currDuration < startDelay) {
                break;
            }
            currState = State::Animating;
            [[fallthrough]];
        }
        case State::Animating: {
            float animTime = currDuration - startDelay;
            float prog = std::min(animTime / duration, 1.f);
            float scale = Math::Lerp(1.f, 0.f, prog);
            tm.scale = Vec3(scale);
            if(animTime < duration) {
                break;
            }
            currState = State::Finishing;
            ET_SendEvent(getEntityId(), &ETRenderNode::ET_hide);
            [[fallthrough]];
        }
        case State::Finishing: {
            if(currDuration < startDelay + duration + endDelay) {
                return;
            }
            currState = State::Ended;
            [[fallthrough]];
        }
        case State::Ended: {
            tm.scale = Vec3(1.f);
            ET_SendEvent(getEntityId(), &ETGameBoardElem::ET_onDestroyPlayed);
            ETNode<ETGameTimerEvents>::disconnect();
            break;
        }
        default: {
            assert(false && "Invalid anim state");
        }
    }

    ET_SendEvent(getEntityId(), &ETEntity::ET_setLocalTransform, tm);
}

void BoardElemDestroyAnimation::setDestroySoundEvent(const char* eventName) {
    ET_SendEventReturn(destroySound, &ETSoundManager::ET_createEvent, eventName);
}

void BoardElemDestroyAnimation::ET_playDestroy() {
    assert(currState == State::Ended && "Invalid anim state");

    currState = State::Starting;
    currDuration = 0.f;

    ETNode<ETGameTimerEvents>::connect(getEntityId());
    destroySound.emit();
}