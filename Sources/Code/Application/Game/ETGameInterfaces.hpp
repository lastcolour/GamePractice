#ifndef __ET_GAME_INTERFACES_HPP__
#define __ET_GAME_INTERFACES_HPP__

struct ETGameScoreUpdater {
    virtual ~ETGameScoreUpdater() = default;
    virtual void ET_reset() = 0;
    virtual void ET_pause() = 0;
    virtual void ET_resume() = 0;
    virtual void ET_setGameScore(int score) = 0;
};

enum class EGameLimitType {
    None = 0,
    Time,
    Moves
};

struct ETGameLimits {
    virtual ~ETGameLimits() = default;
    virtual void ET_setLabelId(EntityId labelId) = 0;
    virtual EGameLimitType ET_getLimitType() const = 0;
};

struct ETGameLimitsEvents {
    virtual ~ETGameLimitsEvents() = default;
    virtual void ET_onGameLimitReached() = 0;
};

#endif /* __ET_GAME_INTERFACES_HPP__ */