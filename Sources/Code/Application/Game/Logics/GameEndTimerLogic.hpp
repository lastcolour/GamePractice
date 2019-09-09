#ifndef __GAME_END_TIMER_LOGIC_HPP__
#define __GAME_END_TIMER_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"

class GameEndTimerLogic : public EntityLogic,
    public ETNode<ETGameEndTimer>,
    public ETNode<ETGameTimerEvents> {
public:
    GameEndTimerLogic();
    virtual ~GameEndTimerLogic();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

    // ETGameEndTimer
    float ET_getRemainingTime() const override;
    float ET_getInitialEndGameDuration() const override;

private:

    float endTime;
    float remainingTime;
};

#endif /* __GAME_END_TIMER_LOGIC_HPP__ */