#ifndef __ET_GAME_INTERFACES_HPP__
#define __ET_GAME_INTERFACES_HPP__

struct ETGameEndTimerUpdater {
    virtual ~ETGameEndTimerUpdater() = default;
    virtual void ET_setEndTime(float endTime) = 0;
};

struct ETGameScoreUpdater {
    virtual ~ETGameScoreUpdater() = default;
    virtual void ET_reset() = 0;
    virtual void ET_pause() = 0;
    virtual void ET_resume() = 0;
    virtual void ET_setGameScore(int score) = 0;
};

struct ETGameEndTimer {
    virtual ~ETGameEndTimer() = default;
    virtual void ET_startEndTimer() = 0;
    virtual float ET_getRemainingTime() const = 0;
    virtual float ET_getInitialEndGameDuration() const = 0;
};

struct ETGameEndTimerEvents {
    virtual ~ETGameEndTimerEvents() = default;
    virtual void ET_onGameTimeOut() = 0;
};

#endif /* __ET_GAME_INTERFACES_HPP__ */