#include "Core/TimePoint.hpp"

TimePoint TimePoint::GetNowTime() {
    TimePoint timePoint;
    timePoint.value =  ClockT::now();
    return timePoint;
}

TimePoint::TimePoint() :
    value() {
}

TimePoint::TimePoint(const TimePoint& other) :
    value(other.value) {
}

TimePoint& TimePoint::operator=(const TimePoint& other) {
    value = other.value;
    return *this;
}

TimePoint::~TimePoint() {
}

float TimePoint::getSecElapsedFrom(const TimePoint& other) const {
    auto msValue = std::chrono::duration_cast<std::chrono::milliseconds>(value - other.value).count();
    auto sValue = static_cast<float>(msValue / 1000.f);
    return sValue;
}

float TimePoint::getMiliSecElapsedFrom(const TimePoint& other) const {
    auto mcsValue = std::chrono::duration_cast<std::chrono::microseconds>(value - other.value).count();
    auto msValue = static_cast<float>(mcsValue / 1000.f);
    return msValue;
}

void TimePoint::addMiliSecDelta(float msDelta) {
    value += std::chrono::microseconds(static_cast<int>(msDelta * 1000.f));
}

void TimePoint::addSecDelta(float secDelta) {
    value += std::chrono::milliseconds(static_cast<int>(secDelta * 1000.f));
}

const typename TimePoint::ClockT::time_point& TimePoint::getStdTimePoint() const {
    return value;
}

typename TimePoint::ClockT::time_point TimePoint::getStdFutureTimePoint(int msOffset) const {
    return value + std::chrono::milliseconds(msOffset);
}