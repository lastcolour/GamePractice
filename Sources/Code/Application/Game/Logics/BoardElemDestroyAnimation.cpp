#include "Game/Logics/BoardElemDestroyAnimation.hpp"
#include "Render/ETRenderNode.hpp"

#include <cassert>

void BoardElemDestroyAnimation::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<BoardElemDestroyAnimation>("BoardElemDestroyAnimation")) {
        classInfo->addField("startDelay", &BoardElemDestroyAnimation::startDelay);
        classInfo->addField("duration", &BoardElemDestroyAnimation::duration);
        classInfo->addField("endDelay", &BoardElemDestroyAnimation::endDelay);
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
    Transform tm = getLocalTransform();
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
            ET_SendEvent(getEntityId(), &ETGameBoardElem::ET_onDestroyDone);
            ETNode<ETGameTimerEvents>::disconnect();
            break;
        }
        default: {
            assert(false && "Invalid anim state");
        }
    }

    setLocalTransform(tm);
}

void BoardElemDestroyAnimation::ET_playDestroy() {
    assert(currState == State::Ended && "Invalid anim state");

    currState = State::Starting;
    currDuration = 0.f;

    ETNode<ETGameTimerEvents>::connect(getEntityId());
}