#ifndef __FUTURE_TIME_POINT_HPP__
#define __FUTURE_TIME_POINT_HPP__

#include <chrono>

class FutureTimePoint {

    using ClockT = std::chrono::high_resolution_clock;

public:

    FutureTimePoint(unsigned int msDelay) {
        resetDelay(msDelay);
    }

    ~FutureTimePoint() = default;

    void resetDelay(unsigned int msDelay) {
        timePoint = ClockT::now() + std::chrono::milliseconds(msDelay);
    }

    void addDelay(unsigned int msDelay) {
        timePoint += std::chrono::milliseconds(msDelay);
    }

    bool isHappened() const {
         return ClockT::now() > timePoint;
    }

private:

    ClockT::time_point timePoint;
};

#endif /* __FUTURE_TIME_POINT_HPP__ */