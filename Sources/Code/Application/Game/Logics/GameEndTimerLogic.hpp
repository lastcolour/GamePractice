#ifndef __GAME_END_TIMER_LOGIC_HPP__
#define __GAME_END_TIMER_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "ETApplicationInterfaces.hpp"

class GameEndTimerLogic : public GameLogic,
    public ETNode<ETTimerEvents> {
public:
    GameEndTimerLogic();
    virtual ~GameEndTimerLogic();

    // GameLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETTimerEvents
    void ET_onTick(float dt) override;

private:

    float endTime;
    float remainingTime;
};

#endif /* __GAME_END_TIMER_LOGIC_HPP__ */