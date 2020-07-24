#ifndef __GAME_TIMER_HPP__
#define __GAME_TIMER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETTasks.hpp"
#include "Game/ETGame.hpp"
#include "Core/ETPrimitives.hpp"

class GameTimer : public SystemLogic,
    public ETNode<ETGameUpdateTask>,
    public ETNode<ETGameTimer> {
public:

    GameTimer();
    virtual ~GameTimer();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETGameUpdateTask
    void ET_updateGame() override;

    // ETGameTimer
    void ET_pauseTimer() override;
    void ET_resumeTimer() override;
    bool ET_isTimerPaused() const override;

private:

    bool skipUpdate;
    bool isPaused;
};

#endif /* __GAME_TIMER_HPP__ */