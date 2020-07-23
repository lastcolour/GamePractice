#ifndef __TIME_POINT_HPP__
#define __TIME_POINT_HPP__

#include <chrono>

class TimePoint {

    using ClockT = std::chrono::high_resolution_clock;

public:

    static TimePoint GetNowTime();

public:

    TimePoint();
    TimePoint(const TimePoint& other);
    TimePoint& operator=(const TimePoint& other);
    ~TimePoint();

    float getSecondsElapsedFrom(const TimePoint& other) const;

private:

    ClockT::time_point value;
};

#endif /* __TIME_POINT_HPP__ */