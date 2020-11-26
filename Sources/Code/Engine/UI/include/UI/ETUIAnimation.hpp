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
    virtual void ET_onAnimationPlayed(EntityId sourceId, EAnimSequenceType animType) = 0;
};

#endif /* __ET_UI_ANIMATION_HPP__ */