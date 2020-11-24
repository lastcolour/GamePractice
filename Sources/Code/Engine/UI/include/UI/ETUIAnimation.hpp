#ifndef __ET_UI_ANIMATION_HPP__
#define __ET_UI_ANIMATION_HPP__

#include "Core/Core.hpp"

class UIAnimationFrame;

enum class EAnimSequenceType {
    Default = 0,
    Press,
    Appear,
    Disappear,
    Highlight
};

struct ETUIViewAppearAnimation {
    virtual ~ETUIViewAppearAnimation() = default;
    virtual void ET_playAppear(EntityId triggerId) = 0;
    virtual void ET_playDissapear(EntityId triggerId) = 0;
};

struct ETUIViewAppearAnimationEvents {
    virtual ~ETUIViewAppearAnimationEvents() = default;
    virtual void ET_onAppearPlayed(EntityId viewId) = 0;
    virtual void ET_onDisappearPlayed(EntityId viewId) = 0;
};

struct ETUIAnimationSequence {
    virtual ~ETUIAnimationSequence() = default;
    virtual bool ET_playAnimation(EntityId triggerId, EAnimSequenceType filter) = 0;
    virtual void ET_addAnimationFrame(const UIAnimationFrame& newFrame) = 0;
    virtual void ET_addSubAnimation(EntityId subAnimId) = 0;
    virtual void ET_stopAnimation(EAnimSequenceType filter) = 0;
    virtual EAnimSequenceType ET_getType() const = 0;
    virtual void ET_setType(EAnimSequenceType newType) = 0;
    virtual void ET_setLooped(bool flag) = 0;
};

struct ETUIAnimationSequenceEvent {
    virtual ~ETUIAnimationSequenceEvent() = default;
    virtual void ET_onAnimationPlayed(EAnimSequenceType animType) = 0;
};

#endif /* __ET_UI_ANIMATION_HPP__ */