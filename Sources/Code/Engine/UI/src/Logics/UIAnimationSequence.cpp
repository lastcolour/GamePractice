#include "Logics/UIAnimationSequence.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Reflect/EnumInfo.hpp"
#include "Math/Primitivies.hpp"
#include "Entity/ETEntity.hpp"
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
    float res = t;
    switch(mode) {
        case EAnimLerpMode::Linear: {
            res = t;
            break;
        }
        case EAnimLerpMode::Pow_2: {
            res = t * t;
            break;
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
            {"Idle", EAnimSequenceType::Idle},
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
    currDuration(0.f),
    startDelay(0.f),
    seqType(EAnimSequenceType::Idle),
    onStartEvent(EShowEvent::None),
    onEndEvent(EShowEvent::None),
    cyclic(false),
    autoStart(false),
    isPlaying(false) {
}

UIAnimationSequence::~UIAnimationSequence() {
}

void UIAnimationSequence::init() {
    if(frames.empty()) {
        return;
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
    ETNode<ETUIElementEvents>::connect(getEntityId());
}

void UIAnimationSequence::ET_onLoaded() {
    ETNode<ETEntityEvents>::disconnect();

    if(!ET_IsExistNode<ETUIElement>(getEntityId())) {
        LogWarning("[UIAnimationSequence::ET_onLoaded] Can't have animation without any UI element: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        return;
    }
    bool isHidden = false;
    ET_SendEventReturn(isHidden, getEntityId(), &ETUIElement::ET_isHidden);
    if(isHidden && !isAnimHasShowEvent()) {
        return;
    }

    if(autoStart) {
        ET_playAnimation(InvalidEntityId);
    }
}

void UIAnimationSequence::deinit() {
    ET_stopAnimation();
}

EAnimSequenceType UIAnimationSequence::ET_getType() const {
    return seqType;
}

void UIAnimationSequence::ET_setType(EAnimSequenceType newType) {
    seqType = newType;
}

void UIAnimationSequence::ET_setStartEndEvents(EShowEvent newStartEvent, EShowEvent newEndEvent) {
    onStartEvent = newStartEvent;
    onEndEvent = newEndEvent;
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
    currUIAddTm.reset();

    float animTime = currDuration - startDelay;
    if(animTime < 0.f) {
        return;
    }

    bool hasActiveFrame = false;

    for(int i = 0, sz = frames.size(); i < sz; ++i) {
        if(animTime <= 0.f) {
            break;
        }
        auto& frame = frames[i];
        if(i == 0 && frame.state == EAnimFrameState::Pending) {
            processShowEvent(getEntityId(), onStartEvent);
        }
        if(frame.state != EAnimFrameState::Finished) {
            hasActiveFrame = true;
        }
        processFrame(animTime, frame);
        animTime -= frame.duration;
    }

    ET_SendEvent(getEntityId(), &ETUIAdditiveAnimationTarget::ET_addAdditiveTransform,
        currUIAddTm);

    if(!hasActiveFrame) {
        auto prevTriggerId = triggerId;
        if(cyclic) {
            restartCycle();
        } else {
            auto prevTriggerId = triggerId;
            ET_stopAnimation();
            ET_SendEvent(prevTriggerId, &ETUIAnimationSequenceEvent::ET_onAnimationPlayed,
                getEntityId(), seqType);
        }
    }
}

void UIAnimationSequence::ET_stopAnimation() {
    if(!isPlaying) {
        return;
    }

    isPlaying = false;

    for(auto& frame : frames) {
        frame.state = EAnimFrameState::Finished;
    }
    for(auto& subAnimId : subAnimations) {
        auto subAnim = UI::GetAnimation(seqType, subAnimId);
        if(subAnim) {
            subAnim->ET_stopAnimation();
        }
    }

    currUIAddTm.inverse();
    ET_SendEvent(getEntityId(), &ETUIAdditiveAnimationTarget::ET_addAdditiveTransform,
        currUIAddTm);
    currUIAddTm.reset();

    processShowEvent(getEntityId(), onEndEvent);

    ETNode<ETUITimerEvents>::disconnect();
}

void UIAnimationSequence::ET_playAnimation(EntityId animTriggerId) {
    if(isPlaying) {
        return;
    }
    if(frames.empty()) {
        LogError("[UIAnimationSequence::ET_playAnimation] No frames to play on: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        return;
    }

    triggerId = animTriggerId;
    isPlaying = true;

    resetAnimState();

    for(auto& subAnimId : subAnimations) {
        auto subAnim = UI::GetAnimation(seqType, subAnimId);
        if(subAnim) {
            subAnim->ET_playAnimation(InvalidEntityId);
        }
    }

    ETNode<ETUITimerEvents>::connect(getEntityId());
    return;
}

void UIAnimationSequence::restartCycle() {
    processShowEvent(getEntityId(), onEndEvent);

    currDuration = 0.f;

    for(auto& frame : frames) {
        frame.state = EAnimFrameState::Pending;
    }

    for(auto& subAnimId : subAnimations) {
        auto subAnim = UI::GetAnimation(seqType, subAnimId);
        if(subAnim) {
            subAnim->ET_stopAnimation();
            subAnim->ET_playAnimation(InvalidEntityId);
        }
    }
}

void UIAnimationSequence::processFrame(float frameTime, UIAnimationFrame& frame) {
    if(frame.state == EAnimFrameState::Finished) {
        return;
    }
    if(frame.state == EAnimFrameState::Pending) {
        frame.state = EAnimFrameState::InProgress;
    }

    float prog = frameTime / frame.duration;
    prog = std::min(prog, 1.f);
    prog = lerpByMode(prog, frame.lerpMode);

    Vec2 newOffset = Math::Lerp(prevState.offset, frame.offset, prog);

    Vec2 pfOffset;
    pfOffset.x = UI::GetValueOnGrind(newOffset.x);
    pfOffset.y = UI::GetValueOnGrind(newOffset.y);

    Vec2 newScale = Math::Lerp(prevState.scale, frame.scale, prog);

    currUIAddTm.tm.pt = Vec3(pfOffset, 0.f);
    currUIAddTm.tm.scale = Vec3(newScale, 1.f);
    currUIAddTm.alpha = Math::Lerp(prevState.alpha, frame.alpha, prog);
    currUIAddTm.alpha = Math::Clamp(currUIAddTm.alpha, 0.001f, 1.f);

    if(frameTime >= frame.duration) {
        prevState.offset = frame.offset;
        prevState.scale = frame.scale;
        prevState.alpha = frame.alpha;
        frame.state = EAnimFrameState::Finished;
    }
}

void UIAnimationSequence::ET_onHidden(bool flag) {
    if(flag && !isAnimHasShowEvent()) {
        ET_stopAnimation();
    }
    if(!flag && autoStart) {
        ET_playAnimation(InvalidEntityId);
    }
}

bool UIAnimationSequence::isAnimHasShowEvent() const {
    return onStartEvent == EShowEvent::Show || onEndEvent == EShowEvent::Show;
}

void UIAnimationSequence::resetAnimState() {
    currDuration = 0.f;
    prevState.offset = Vec2(0.f);
    prevState.scale = Vec2(1.f);
    prevState.alpha = 1.f;
    for(auto& frame : frames) {
        frame.state = EAnimFrameState::Pending;
    }
}

namespace UI {

bool PlayAnimation(EntityId entityId, EAnimSequenceType type, EntityId triggerId) {
    auto anim = UI::GetAnimation(type, entityId);
    if(!anim) {
        return false;
    }
    anim->ET_playAnimation(triggerId);
    return true;
}

} // namespace UI