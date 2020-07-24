#ifndef __ET_UI_TIMER_HPP__
#define __ET_UI_TIMER_HPP__

struct ETUITimerEvents {
    virtual ~ETUITimerEvents() = default;
    virtual void ET_onUITick(float dt) = 0; 
};

#endif /* __ET_UI_TIMER_HPP__ */