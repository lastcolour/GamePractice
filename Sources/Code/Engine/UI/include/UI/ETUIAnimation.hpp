#ifndef __ET_UI_ANIMATION_HPP__
#define __ET_UI_ANIMATION_HPP__

struct ETUIAnimation {
    virtual ~ETUIAnimation() = default;
    virtual void ET_start() = 0;
    virtual void ET_startReverse();
    virtual void ET_pause() = 0;
    virtual void ET_resume() = 0;
    virtual void ET_reverse() = 0;
    virtual bool ET_isReversed() const = 0;
    virtual float ET_getDuration() const = 0;
};

struct ETUIAnimationEvents {
    virtual ~ETUIAnimationEvents() = default;
    virtual void ET_onAnimationEnd() = 0;
};

#endif /* __ET_UI_ANIMATION_HPP__ */