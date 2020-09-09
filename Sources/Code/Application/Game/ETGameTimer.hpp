#ifndef __ET_GAME_TIMER_HPP__
#define __ET_GAME_TIMER_HPP__

struct ETGameTimer {
    virtual ~ETGameTimer() = default;
    virtual void ET_onTick() = 0;
    virtual void ET_setScale(float newScale) = 0;
    virtual void ET_pause() = 0;
    virtual void ET_resume() = 0;
    virtual bool ET_isPaused() const = 0;
};

struct ETGameTimerEvents {
    virtual ~ETGameTimerEvents() = default;
    virtual void ET_onGameTick(float dt) = 0;
};

#endif /* __ET_GAME_TIMER_HPP__ */