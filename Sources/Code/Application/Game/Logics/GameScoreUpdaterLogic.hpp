#ifndef __GAME_SCORE_UPDATER_LOGIC_HPP__
#define __GAME_SCORE_UPDATER_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"

class GameScoreUpdaterLogic : public EntityLogic,
    public ETNode<ETGameScoreUpdater>,
    public ETNode<ETGameTimerEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameScoreUpdaterLogic();
    virtual ~GameScoreUpdaterLogic();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETGameScoreUpdater
    void ET_setGameScore(int score) override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

private:

    float currentStepDelay;
    int increaseSpeed;
    int currentValue;
    int targetValue;
};

#endif /* __GAME_SCORE_UPDATER_LOGIC_HPP__ */