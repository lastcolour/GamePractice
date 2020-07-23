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

float TimePoint::getSecondsElapsedFrom(const TimePoint& other) const {
    auto msValue = std::chrono::duration_cast<std::chrono::milliseconds>(value - other.value).count();
    auto sValue = static_cast<float>(msValue / 1000.f);
    return sValue;
}