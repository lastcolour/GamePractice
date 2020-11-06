#ifndef __ET_GAME_SCORE_HPP__
#define __ET_GAME_SCORE_HPP__

enum class ObjectiveProgress {
    Fail = 0,
    OneStar,
    TwoStars,
    ThreeStars
};

struct ETGameScore {
    virtual ~ETGameScore() = default;
    virtual void ET_resetScore() = 0;
    virtual int ET_getGameScore() const = 0;
    virtual ObjectiveProgress ET_getObjectiveProgress() const = 0;
};

struct ETGameObjectiveEvents {
    virtual ~ETGameObjectiveEvents() = default;
    virtual void ET_onObjectiveCompleted(ObjectiveProgress type) = 0;
};

struct EndGameResult {
    float gameTime;
    int score;
    ObjectiveProgress objectiveCompleted;
    bool newHighScore;

    EndGameResult() :
        gameTime(0.f),
        score(0),
        objectiveCompleted(ObjectiveProgress::Fail),
        newHighScore(false) {}
};

struct ETGameEndResult {
    virtual ~ETGameEndResult() = default;
    virtual const EndGameResult& ET_getLastGameResult() const = 0;
};

#endif /* __ET_GAME_SCORE_HPP__ */