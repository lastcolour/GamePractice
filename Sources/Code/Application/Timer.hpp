#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include <memory>

struct TimePoint;

class Timer {
public:

    Timer();
    ~Timer();

    float tick();
    float getFrameTime() const;

private:

    std::unique_ptr<TimePoint> timePoint;
};

#endif /* __TIMER_HPP__ */