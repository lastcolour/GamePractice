#ifndef __GAME_END_TIMER_LOGIC_HPP__
#define __GAME_END_TIMER_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "ETApplicationInterfaces.hpp"

class GameEndTimerLogic : public EntityLogic,
    public ETNode<ETTimerEvents> {
public:
    GameEndTimerLogic();
    virtual ~GameEndTimerLogic();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETTimerEvents
    void ET_onTick(float dt) override;

private:

    std::string endEvent;
    float endTime;
    float remainingTime;
};

#endif /* __GAME_END_TIMER_LOGIC_HPP__ */