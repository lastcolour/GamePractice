#ifndef __ET_TIMER_HPP__
#define __ET_TIMER_HPP__

struct ETMainThreadTimer {
    virtual ~ETMainThreadTimer() = default;
    virtual void ET_onMainThreadStep() = 0;
    virtual void ET_setAppTimeScale(float newScale) = 0;
};

struct ETSystemTimerEvents {
    virtual ~ETSystemTimerEvents() = default;
    virtual void ET_onSystemTick(float dt) = 0;
};

struct ETAppTimerEvents {
    virtual ~ETAppTimerEvents() = default;
    virtual void ET_onAppTick(float dt) = 0;
};

#endif /* __ET_TIMER_HPP__ */