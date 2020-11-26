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

void processShowEvent(EntityId targetId, EShowEvent event) {
    switch(event) {
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
    if(auto enumInfo = ctx.enumInfo<EShowEvent>("EShowEvent")) {
        enumInfo->addValues<EShowEvent>({
            {"None", EShowEvent::None},
            {"Hide", EShowEvent::Hide},
            {"Show", EShowEvent::Show}
        });
    }
    if(auto classInfo = ctx.classInfo<UIAnimationSequence>("UIAnimationSequence")) {
        classInfo->addField("autoStart", &UIAnimationSequence::autoStart);
        classInfo->addField("cyclic", &UIAnimationSequence::cyclic);
        classInfo->addField("type", &UIAnimationSequence::seqType);
        classInfo->addField("startDelay", &UIAnimationSequence::startDelay);
        classInfo->addField("subAnimations", &UIAnimationSequence::subAnimations);
        classInfo->addField("startEvent", &UIAnimationSequence::onStartEvent),
        classInfo->addField("endEvent", &UIAnimationSequence::onEndEvent),
        classInfo->addField("frames", &UIAnimationSequence::frames);
    }
}

UIAnimationSequence::UIAnimationSequence() :
    origAlpha(1.f),
    currDuration(0.f),
    startDelay(0.f),
    seqType(EAnimSequenceType::Default),
    onStartEvent(EShowEvent::None),
    onEndEvent(EShowEvent::None),
    cyclic(false),
    autoStart(false),
    isPlaying(false),
    isOrigTmValid(false) {
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

    ETNode<ETEntityEvents>::connect(getEntityId());
    ETNode<ETUIAnimationSequence>::connect(getEntityId());
    return true;
}

void UIAnimationSequence::ET_onTransformChanged(const Transform& newTm) {
}

void UIAnimationSequence::ET_onLoaded() {
    if(autoStart) {
        ET_playAnimation(InvalidEntityId, seqType);
    }
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
    if(!isOrigTmValid) {
        isOrigTmValid = true;
        ET_SendEventReturn(origTm, getEntityId(), &ETEntity::ET_getLocalTransform);
        ET_SendEventReturn(origAlpha, getEntityId(), &ETUIElement::ET_getAlpha);

        prevState.offset = Vec2(0.f);
        prevState.scale = Vec2(origTm.scale.x, origTm.scale.y);
        prevState.alpha = origAlpha;
    }

    currDuration += dt;
    float animTime = currDuration - startDelay;

    assert(!frames.empty() && "frame list empty");

    for(int i = 0, sz = frames.size(); i < sz; ++i) {
        if(animTime <= 0.f) {
            break;
        }
        auto& frame = frames[i];
        if(i == 0 && frame.state == EAnimFrameState::Pending) {
            processShowEvent(getEntityId(), onStartEvent);
            for(auto& subAnimId : subAnimations) {
                ET_SendEvent(subAnimId, &ETUIAnimationSequence::ET_playAnimation, InvalidEntityId, seqType);
                ET_SendEvent(subAnimId, &ETUITimerEvents::ET_onUITick, dt);
            }
        }
        processFrame(animTime, frame);
        animTime -= frame.duration;
    }

    if(frames.back().state == EAnimFrameState::Finished) {
        UI::SetLocalTMDoNotUpdateLayout(getEntityId(), origTm);
        ET_SendEvent(getEntityId(), &ETUIElement::ET_setAlpha, origAlpha);
        if(cyclic) {
            for(auto& subAnimId : subAnimations) {
                ET_SendEvent(subAnimId, &ETUIAnimationSequence::ET_stopAnimation, seqType);
            }
            processShowEvent(getEntityId(), onEndEvent);
            reStartCycle();
        } else {
            ET_stopAnimation(seqType);
            ET_SendEvent(triggerId, &ETUIAnimationSequenceEvent::ET_onAnimationPlayed,
                getEntityId(), seqType);
            triggerId = InvalidEntityId;
        }
    }
}

void UIAnimationSequence::ET_stopAnimation(EAnimSequenceType filter) {
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

    processShowEvent(getEntityId(), onEndEvent);

    ETNode<ETUITimerEvents>::disconnect();
}

void UIAnimationSequence::reStartCycle() {
    currDuration = 0.f;
    prevState.offset = Vec2(0.f);
    prevState.scale = Vec2(origTm.scale.x, origTm.scale.y);
    prevState.alpha = origAlpha;
    for(auto& frame : frames) {
        frame.state = EAnimFrameState::Pending;
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
    isOrigTmValid = false;
    currDuration = 0.f;

    for(auto& frame : frames) {
        frame.state = EAnimFrameState::Pending;
    }

    ETNode<ETUITimerEvents>::connect(getEntityId());
    return true;
}

void UIAnimationSequence::processFrame(float dt, UIAnimationFrame& frame) {
    if(frame.state == EAnimFrameState::Finished) {
        return;
    }
    if(frame.state == EAnimFrameState::Pending) {
        frame.state = EAnimFrameState::InProgress;
    }

    float prog = dt / frame.duration;
    prog = std::min(prog, 1.f);
    prog = lerpByMode(prog, frame.lerpMode);

    Vec2 newOffset = Math::Lerp(prevState.offset, frame.offset, prog);

    Vec2 pfOffset;
    pfOffset.x = UI::GetValueOnGrind(newOffset.x);
    pfOffset.y = UI::GetValueOnGrind(newOffset.y);

    Vec2 newScale = Math::Lerp(prevState.scale, frame.scale, prog);

    auto tm = origTm;
    tm.pt += Vec3(pfOffset, 0.f);
    tm.scale = Vec3(newScale, origTm.scale.z);
    UI::SetLocalTMDoNotUpdateLayout(getEntityId(), tm);

    float newAlpha = Math::Lerp(prevState.alpha, frame.alpha, prog);
    ET_SendEvent(getEntityId(), &ETUIElement::ET_setAlpha, newAlpha);

    if(dt >= frame.duration) {
        prevState.offset = frame.offset;
        prevState.scale = frame.scale;
        prevState.alpha = frame.alpha;
        frame.state = EAnimFrameState::Finished;
    }
}