#ifndef __GAME_TIMER_HPP__
#define __GAME_TIMER_HPP__

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"

class GameTimer : public SystemLogic,
    public ETNode<ETTimerEvents>,
    public ETNode<ETGameTimer> {
public:

    GameTimer();
    virtual ~GameTimer();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETTimerEvents
    void ET_onTick(float dt) override;

    // ETGameTimer
    void ET_pauseTimer() override;
    void ET_resumeTimer() override;

private:

    bool isPaused;
};

#endif /* __GAME_TIMER_HPP__ */