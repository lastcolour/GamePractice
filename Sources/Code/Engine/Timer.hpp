#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"

#include <chrono>

class Timer : public SystemLogic,
    public ETNode<ETTimer> {

    using ClockT = std::chrono::high_resolution_clock;
    using TimePointT = std::chrono::time_point<ClockT>;

public:

    Timer();
    virtual ~Timer();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETTimer
    void ET_onFrameStart() override;

private:

    TimePointT lastTickT;
    float tickDuration;
};

#endif /* __TIMER_HPP__ */