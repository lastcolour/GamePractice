#ifndef __GAME_SCORE_UPDATER_LOGIC_HPP__
#define __GAME_SCORE_UPDATER_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGameTimer.hpp"
#include "Audio/SoundEvent.hpp"

class GameScoreUpdaterLogic : public EntityLogic,
    public ETNode<ETGameScoreUpdater>,
    public ETNode<ETGameTimerEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameScoreUpdaterLogic();
    virtual ~GameScoreUpdaterLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameScoreUpdater
    void ET_reset() override;
    void ET_pause() override;
    void ET_resume() override;
    void ET_setGameScore(int score) override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

private:

    SoundEvent increaseSound;
    EntityId amountLabelId;
    EntityId progressBardId;
    float currentStepDelay;
    int increaseSpeed;
    int currentValue;
    int targetValue;
};

#endif /* __GAME_SCORE_UPDATER_LOGIC_HPP__ */