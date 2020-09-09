#ifndef __ET_UI_TIMER_HPP__
#define __ET_UI_TIMER_HPP__

struct ETUITimerEvents {
    virtual ~ETUITimerEvents() = default;
    virtual void ET_onUITick(float dt) = 0;
};

struct ETUITimer {
    virtual ~ETUITimer() = default;
    virtual void ET_onTick() = 0;
    virtual void ET_setScale(float newScale) = 0;
    virtual void ET_pause() = 0;
    virtual void ET_resume() = 0;
    virtual bool ET_isPaused() const = 0;
};

#endif /* __ET_UI_TIMER_HPP__ */