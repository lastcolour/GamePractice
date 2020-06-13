#ifndef __ET_TIMER_HPP__
#define __ET_TIMER_HPP__

struct ETTimer {
    virtual ~ETTimer() = default;
    virtual void ET_onFrameStart() = 0;
};

struct ETTimerEvents {
    virtual ~ETTimerEvents() = default;
    virtual void ET_onTick(float dt) = 0;
};

#endif /* __ET_TIMER_HPP__ */