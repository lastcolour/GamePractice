#include "Game/Logics/BoardElemSelectAnimation.hpp"
#include "Render/ETRenderNode.hpp"

void BoardElemSelectAnimation::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<BoardElemSelectAnimation>("BoardElemSelectAnimation")) {
        classInfo->addField("duration", &BoardElemSelectAnimation::duration);
        classInfo->addField("maxBackgroundScale", &BoardElemSelectAnimation::maxBackgroundScale);
        classInfo->addField("minForegroundScale", &BoardElemSelectAnimation::minForegroundScale);
        classInfo->addField("background", &BoardElemSelectAnimation::backgroundId);
    }
}

BoardElemSelectAnimation::BoardElemSelectAnimation() :
    currDuration(-1.f),
    duration(0.1f),
    maxBackgroundScale(1.2f),
    minForegroundScale(0.8f),
    state(State::None) {
}

BoardElemSelectAnimation::~BoardElemSelectAnimation() {
}

void BoardElemSelectAnimation::init() {
    ETNode<ETGameBoardElemSelectAnimation>::connect(getEntityId());
}

void BoardElemSelectAnimation::deinit() {
    if(state != State::None) {
        state = State::Deselect;
        ET_onGameTick(1024.f);
    }
}

void BoardElemSelectAnimation::updateBackground(float prog) {
    auto scale = Math::Lerp(1.f, minForegroundScale, prog);
    ET_SendEvent(getEntityId(), &ETRenderRect::ET_setSize, startSize * scale);
}

void BoardElemSelectAnimation::updateForeground(float prog) {
    auto scale = Math::Lerp(1.f, maxBackgroundScale, prog);
    ET_SendEvent(backgroundId, &ETRenderRect::ET_setSize, startSize * scale);
}

void BoardElemSelectAnimation::ET_onGameTick(float dt) {
    currDuration += dt;
    auto prog = std::min(1.f, currDuration / duration);
    if(state == State::Deselect) {
        prog = 1.f - prog;
    }
    updateBackground(prog);
    updateForeground(prog);
    if(currDuration >= duration) {
        if(state == State::Deselect) {
            state = State::None;
            ET_SendEvent(backgroundId, &ETRenderNode::ET_hide);
        }
        currDuration = 0.f;
        ETNode<ETGameTimerEvents>::disconnect();
    }
}

void BoardElemSelectAnimation::ET_playSelect() {
    if(state == State::Deselect) {
        currDuration = duration - currDuration;
        state = State::Select;
    } else if(state == State::None) {
        state = State::Select;
        currDuration = 0.f;

        ET_SendEvent(backgroundId, &ETRenderNode::ET_show);

        int zIndex = 0;
        ET_SendEventReturn(zIndex, getEntityId(), &ETRenderNode::ET_getZIndex);
        ET_SendEvent(backgroundId, &ETRenderNode::ET_setZIndex, zIndex - 1);

        ET_SendEventReturn(startSize, getEntityId(), &ETRenderRect::ET_getSize);
        ET_SendEvent(backgroundId, &ETRenderRect::ET_setSize, startSize);

        ETNode<ETGameTimerEvents>::connect(getEntityId());
    }
}

void BoardElemSelectAnimation::ET_playDeselect() {
    if(state != State::Select) {
        LogWarning("[BoardElemSelectAnimation::ET_playDeselect] Element isn't selected");
        return;
    } else {
        state = State::Deselect;
        if(currDuration > 0.f) {
            currDuration = duration - currDuration;
        }
        ETNode<ETGameTimerEvents>::connect(getEntityId());
    }
}