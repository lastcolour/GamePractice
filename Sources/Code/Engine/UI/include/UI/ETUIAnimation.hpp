#ifndef __ET_UI_ANIMATION_HPP__
#define __ET_UI_ANIMATION_HPP__

struct ETUIAnimation {
    virtual ~ETUIAnimation() = default;
    virtual void ET_start() = 0;
    virtual float ET_getDuration() const = 0;
};

struct ETUIAnimationEvents {
    virtual ~ETUIAnimationEvents() = default;
    virtual void ET_onAnimationEnd() = 0;
};

#endif /* __ET_UI_ANIMATION_HPP__ */