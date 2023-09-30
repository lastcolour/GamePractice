#ifndef __TIME_POINT_HPP__
#define __TIME_POINT_HPP__

class TimePoint {
public:

    using ClockT = std::chrono::high_resolution_clock;

public:

    static TimePoint GetNow();

public:

    TimePoint();
    TimePoint(const TimePoint& other);
    TimePoint& operator=(const TimePoint& other);
    ~TimePoint();

    float getMsDeltaWith(const TimePoint& other) const;
    float getSecDeltaWith(const TimePoint& other) const;

    float getMsDeltaWithNow() const;
    float getSecDeltaWithNow() const;

    void addMicroSecDelta(float mcsDelta);
    void addMiliSecDelta(float msDelta);
    void addSecDelta(float secDelta);

    const ClockT::time_point& getStdTimePoint() const;
    ClockT::time_point getStdFutureTimePoint(int msOffset) const;

private:

    ClockT::time_point value;
};

#endif /* __TIME_POINT_HPP__ */