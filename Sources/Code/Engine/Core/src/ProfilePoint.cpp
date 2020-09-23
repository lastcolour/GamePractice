#include "Core/ProfilePoint.hpp"
#include "Core/GlobalEnvironment.hpp"
#include "Profiler.hpp"

ProfilePoint::ProfilePoint(const char* pointName) {
    GetEnv()->GetProfiler()->enterSection(pointName);
}

ProfilePoint::~ProfilePoint() {
    GetEnv()->GetProfiler()->leaveSection();
}