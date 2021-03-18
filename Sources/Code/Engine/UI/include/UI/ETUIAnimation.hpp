#ifndef __ET_UI_ANIMATION_HPP__
#define __ET_UI_ANIMATION_HPP__

class UIAnimationFrame;

enum class EAnimSequenceType {
    Idle,
    Press,
    Appear,
    Disappear,
    Highlight
};

enum class EShowEvent {
    None = 0,
    Show,
    Hide
};

struct ETUIAnimationSequence {
    virtual ~ETUIAnimationSequence() = default;
    virtual void ET_playAnimation(EntityId triggerId) = 0;
    virtual void ET_addAnimationFrame(const UIAnimationFrame& newFrame) = 0;
    virtual void ET_addSubAnimation(EntityId subAnimId) = 0;
    virtual void ET_stopAnimation() = 0;
    virtual EAnimSequenceType ET_getType() const = 0;
    virtual void ET_setType(EAnimSequenceType newType) = 0;
    virtual void ET_setLooped(bool flag) = 0;
    virtual void ET_setStartEndEvents(EShowEvent newStartEvent, EShowEvent newEndEvent) = 0;
    virtual bool ET_isPlaying() const = 0;
};

struct ETUIAnimationSequenceEvent {
    virtual ~ETUIAnimationSequenceEvent() = default;
    virtual void ET_onAnimationPlayed(EntityId sourceId, EAnimSequenceType animType) = 0;
};

namespace UI {

bool PlayAnimation(EntityId entityId, EAnimSequenceType type, EntityId triggerId);

} // namespace UI

#endif /* __ET_UI_ANIMATION_HPP__ */