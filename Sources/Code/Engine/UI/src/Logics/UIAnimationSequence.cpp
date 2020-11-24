#include "Logics/UIAnimationSequence.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Reflect/EnumInfo.hpp"
#include "Math/Primitivies.hpp"
#include "Entity/ETEntity.hpp"
#include "UI/ETUIBox.hpp"
#include "UIUtils.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>
#include <cmath>
#include <algorithm>

namespace {

void processFrameStartEvent(EntityId targetId, UIAnimationFrame& frame) {
    switch(frame.onStartEvent) {
        case EShowEvent::None: {
            break;
        }
        case EShowEvent::Hide: {
            ET_SendEvent(targetId, &ETUIElement::ET_hide);
            break;
        }
        case EShowEvent::Show: {
            ET_SendEvent(targetId, &ETUIElement::ET_show);
            break;
        }
        default: {
            assert(false && "Invalid event type");
        }
    }
}

void processFrameEndEvent(EntityId targetId, UIAnimationFrame& frame) {
    switch(frame.onEndEvent) {
        case EShowEvent::None: {
            break;
        }
        case EShowEvent::Hide: {
            ET_SendEvent(targetId, &ETUIElement::ET_hide);
            break;
        }
        case EShowEvent::Show: {
            ET_SendEvent(targetId, &ETUIElement::ET_show);
            break;
        }
        default: {
            assert(false && "Invalid event type");
        }
    }
}

float lerpByMode(float t, EAnimLerpMode mode) {
    assert((t >= 0.f && t <= 1.f) && "value of out range");
    float res;
    switch(mode) {
        case EAnimLerpMode::Linear: {
            res = t;
            break;
        }
        case EAnimLerpMode::Pow_2: {
            res = t * t;
        }
        case EAnimLerpMode::Pow_4: {
            res = t * t;
            res *= res;
            break;
        }
        case EAnimLerpMode::InvPow_2: {
            res = sqrt(t);
            break;
        }
        case EAnimLerpMode::InvPow_4: {
            res = sqrt(t);
            res = sqrt(res);
            break;
        }
        case EAnimLerpMode::Sin: {
            res = sin(t * Math::PI / 2.f);
            break;
        }
        case EAnimLerpMode::Sin_Sqr_2: {
            res = sin(t * Math::PI / 2.f);
            res *= res;
            break;
        }
        case EAnimLerpMode::InvSin_Sqr_2: {
            res = sin(t * Math::PI / 2.f);
            res = sqrt(res);
            break;
        }
        default: {
            assert(false && "Invalid lerp mode");
        }
    }
    res = Math::Clamp(res, 0.f, 1.f);
    return res;
}

} // namespace

void UIAnimationSequence::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<EAnimSequenceType>("EAnimSequenceType")) {
        enumInfo->addValues<EAnimSequenceType>({
            {"Default", EAnimSequenceType::Default},
            {"Appear", EAnimSequenceType::Appear},
            {"Disappear", EAnimSequenceType::Disappear},
            {"Press", EAnimSequenceType::Press},
            {"Highlight", EAnimSequenceType::Highlight}
        });
    }
    if(auto classInfo = ctx.classInfo<UIAnimationSequence>("UIAnimationSequence")) {
        classInfo->addField("autoStart", &UIAnimationSequence::autoStart);
        classInfo->addField("cyclic", &UIAnimationSequence::cyclic);
        classInfo->addField("type", &UIAnimationSequence::seqType);
        classInfo->addField("startDelay", &UIAnimationSequence::startDelay);
        classInfo->addField("subAnimations", &UIAnimationSequence::subAnimations);
        classInfo->addField("frames", &UIAnimationSequence::frames);
    }
}

UIAnimationSequence::UIAnimationSequence() :
    origAlpha(1.f),
    currDuration(0.f),
    startDelay(0.f),
    seqType(EAnimSequenceType::Default),
    cyclic(false),
    autoStart(false),
    isPlaying(false) {
}

UIAnimationSequence::~UIAnimationSequence() {
}

bool UIAnimationSequence::init() {
    if(frames.empty()) {
        return true;
    }

    if(startDelay < 0.f) {
        LogWarning("[UIAnimationSequence::init] Animation sequence has negative start delay: %.2f",
            startDelay, EntityUtils::GetEntityName(getEntityId()));
        startDelay = 0.f;
    }

    for(auto& frame : frames) {
        if(frame.duration < 0.f) {
            LogWarning("[UIAnimationSequence::init] One of the frames has negative duration: %.2f",
                frame.duration, EntityUtils::GetEntityName(getEntityId()));
            frame.duration = 0.f;
        }
    }

    ETNode<ETUIAnimationSequence>::connect(getEntityId());
    if(autoStart) {
        ET_playAnimation(InvalidEntityId, seqType);
    }

    return true;
}

void UIAnimationSequence::deinit() {
    ET_stopAnimation(seqType);
}

EAnimSequenceType UIAnimationSequence::ET_getType() const {
    return seqType;
}

void UIAnimationSequence::ET_setType(EAnimSequenceType newType) {
    seqType = newType;
}

void UIAnimationSequence::ET_setLooped(bool flag) {
    cyclic = flag;
}

void UIAnimationSequence::ET_addAnimationFrame(const UIAnimationFrame& newFrame) {
    frames.push_back(newFrame);
    if(frames.size() == 1) {
        init();
    }
}

void UIAnimationSequence::ET_addSubAnimation(EntityId subAnimId) {
    subAnimations.push_back(subAnimId);
}

void UIAnimationSequence::ET_onUITick(float dt) {
    currDuration += dt;
    if(currDuration <= startDelay) {
        return;
    }
    float animTime = currDuration - startDelay;
    for(auto& frame : frames) {
        processFrame(animTime, frame);
        animTime -= frame.duration;
        if(animTime < 0.f) {
            break;
        }
    }

    assert(!frames.empty() && "frame list empty");

    if(frames.back().state == EAnimFrameState::Finished) {
        UI::SetLocalTMDoNotUpdateLayout(getEntityId(), origTm);
        ET_SendEvent(getEntityId(), &ETUIElement::ET_setAlpha, origAlpha);
        if(cyclic) {
            for(auto& subAnimId : subAnimations) {
                ET_SendEvent(subAnimId, &ETUIAnimationSequence::ET_stopAnimation, seqType);
            }
            startCycle();
        } else {
            ET_stopAnimation(seqType);
            ET_SendEvent(triggerId, &ETUIAnimationSequenceEvent::ET_onAnimationPlayed, seqType);
            triggerId = InvalidEntityId;
        }
    }
}

void UIAnimationSequence::ET_stopAnimation(EAnimSequenceType filter) {
    LogDebug("[ET_stopAnimation] Entity: '%s'", EntityUtils::GetEntityName(getEntityId()));
    if(seqType != filter && seqType != EAnimSequenceType::Default) {
        return;
    }
    if(!isPlaying) {
        return;
    }

    isPlaying = false;
    ET_SendEvent(getEntityId(), &ETUIElement::ET_setAlpha, origAlpha);
    UI::SetLocalTMDoNotUpdateLayout(getEntityId(), origTm);

    for(auto& frame : frames) {
        frame.state = EAnimFrameState::Finished;
    }
    for(auto& subAnimId : subAnimations) {
        ET_SendEvent(subAnimId, &ETUIAnimationSequence::ET_stopAnimation, filter);
    }

    ETNode<ETUITimerEvents>::disconnect();
}

void UIAnimationSequence::startCycle() {
    currDuration = 0.f;
    prevState.tm = origTm;
    prevState.alpha = origAlpha;
    for(auto& frame : frames) {
        frame.state = EAnimFrameState::Pending;
    }
    for(auto& subAnimId : subAnimations) {
        ET_SendEvent(subAnimId, &ETUIAnimationSequence::ET_playAnimation, InvalidEntityId, seqType);
    }
}

bool UIAnimationSequence::ET_playAnimation(EntityId animTriggerId, EAnimSequenceType filter) {
    if(seqType != filter && seqType != EAnimSequenceType::Default) {
        return false;
    }
    if(isPlaying) {
        return true;
    }

    triggerId = animTriggerId;
    isPlaying = true;

    ET_SendEventReturn(origTm, getEntityId(), &ETEntity::ET_getLocalTransform);
    ET_SendEventReturn(origAlpha, getEntityId(), &ETUIElement::ET_getAlpha);

    startCycle();

    ETNode<ETUITimerEvents>::connect(getEntityId());
    return true;
}

void UIAnimationSequence::processFrame(float dt, UIAnimationFrame& frame) {
    if(frame.state == EAnimFrameState::Finished) {
        return;
    }
    if(frame.state == EAnimFrameState::Pending) {
        frame.state = EAnimFrameState::InProgress;
        processFrameStartEvent(getEntityId(), frame);
    }

    auto tm = prevState.tm;
    Vec2 prevScale = Vec2(tm.scale.x, tm.scale.y);

    float prog = dt / frame.duration;
    prog = std::min(prog, 1.f);
    prog = lerpByMode(prog, frame.lerpMode);

    Vec2 newOffset = Math::Lerp(Vec2(0), frame.offset, prog);
    newOffset.x = UI::GetValueOnGrind(newOffset.x);
    newOffset.y = UI::GetValueOnGrind(newOffset.y);

    Vec2 newScale = Math::Lerp(prevScale, frame.scale, prog);

    tm.pt += Vec3(newOffset, 0.f);
    tm.scale = Vec3(newScale, prevState.tm.scale.z);
    UI::SetLocalTMDoNotUpdateLayout(getEntityId(), tm);

    float currAlpha = Math::Lerp(prevState.alpha, frame.alpha, prog);
    ET_SendEvent(getEntityId(), &ETUIElement::ET_setAlpha, currAlpha);

    if(dt >= frame.duration) {
        prevState.tm = tm;
        prevState.alpha = currAlpha;
        frame.state = EAnimFrameState::Finished;
        processFrameEndEvent(getEntityId(), frame);
    }
}