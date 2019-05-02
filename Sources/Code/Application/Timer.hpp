#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"

#include <memory>

struct TimePoint;

class Timer : public SystemLogic,
    public ETNode<ETTimer> {
public:

    Timer();
    virtual ~Timer();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETTimer
    void ET_onFrameStart() override;

private:

    std::unique_ptr<TimePoint> timePoint;
};

#endif /* __TIMER_HPP__ */