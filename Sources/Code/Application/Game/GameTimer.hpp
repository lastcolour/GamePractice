#ifndef __GAME_TIMER_HPP__
#define __GAME_TIMER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETTasks.hpp"
#include "Game/ETGameTimer.hpp"
#include "Core/TimePoint.hpp"

class GameTimer : public SystemLogic,
    public ETNode<ETGameTimer> {
public:

    GameTimer();
    virtual ~GameTimer();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETGameTimer
    void ET_onTick(float dt) override;
    void ET_setScale(float newScale) override;
    void ET_pause() override;
    void ET_resume() override;
    bool ET_isPaused() const override;

private:

    float scale;
    bool skipUpdate;
    bool isPaused;
};

#endif /* __GAME_TIMER_HPP__ */