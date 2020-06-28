#ifndef __MAIN_TIMER_HPP__
#define __MAIN_TIMER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETTimer.hpp"
#include "Core/ETPrimitives.hpp"

#include <chrono>

class MainTimer : public SystemLogic,
    public ETNode<ETMainThreadTimer> {

    using ClockT = std::chrono::high_resolution_clock;
    using TimePointT = std::chrono::time_point<ClockT>;

public:

    MainTimer();
    virtual ~MainTimer();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETMainThreadTimer
    void ET_onMainThreadStep() override;
    void ET_setAppTimeScale(float newScale) override;

private:

    TimePointT lastTickT;
    float appTimeScale;
};

#endif /* __MAIN_TIMER_HPP__ */