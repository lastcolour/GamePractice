#ifndef __TIME_POINT_HPP__
#define __TIME_POINT_HPP__

class TimePoint {
public:

    using ClockT = std::chrono::high_resolution_clock;

public:

    static TimePoint GetNowTime();

public:

    TimePoint();
    TimePoint(const TimePoint& other);
    TimePoint& operator=(const TimePoint& other);
    ~TimePoint();

    float getSecElapsedFrom(const TimePoint& other) const;
    float getMiliSecElapsedFrom(const TimePoint& other) const;

    void addMiliSecDelta(float msDelta);
    void addSecDelta(float secDelta);

    const ClockT::time_point& getStdTimePoint() const;
    ClockT::time_point getStdFutureTimePoint(int msOffset) const;

private:

    ClockT::time_point value;
};

#endif /* __TIME_POINT_HPP__ */