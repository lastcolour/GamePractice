#ifndef __GAME_END_TIMER_LOGIC_HPP__
#define __GAME_END_TIMER_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGameTimer.hpp"

class GameEndTimerLogic : public EntityLogic,
    public ETNode<ETGameEndTimer>,
    public ETNode<ETGameTimerEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:
    GameEndTimerLogic();
    virtual ~GameEndTimerLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

    // ETGameEndTimer
    void ET_startEndTimer() override;
    float ET_getRemainingTime() const override;
    float ET_getInitialEndGameDuration() const override;

private:

    float endTime;
    float remainingTime;
};

#endif /* __GAME_END_TIMER_LOGIC_HPP__ */