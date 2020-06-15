#ifndef __GAME_TIMER_HPP__
#define __GAME_TIMER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETTimer.hpp"
#include "Game/ETGameInterfaces.hpp"

class GameTimer : public SystemLogic,
    public ETNode<ETAppTimerEvents>,
    public ETNode<ETGameTimer> {
public:

    GameTimer();
    virtual ~GameTimer();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETAppTimerEvents
    void ET_onAppTick(float dt) override;

    // ETGameTimer
    void ET_pauseTimer() override;
    void ET_resumeTimer() override;
    bool ET_isTimerPaused() const override;

private:

    bool skipUpdate;
    bool isPaused;
};

#endif /* __GAME_TIMER_HPP__ */