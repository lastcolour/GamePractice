#include "Parallel/TaskRunInfo.hpp"

namespace {

const int MAX_TIME_RECORDS = 240;

} // namespace

TaskRunInfo::TaskRunInfo() :
    timing(MAX_TIME_RECORDS),
    runCount(0) {
}

TaskRunInfo::~TaskRunInfo() {
}

float GetRunTimeBetween(const CycleArray<TaskRunInfo::StartEndTime>& timing, size_t i) {
    return timing[i].startT.getMiliSecElapsedFrom(timing[i - 1].startT);
}

float GetRunDurationAt(const CycleArray<TaskRunInfo::StartEndTime>& timing, size_t i) {
    return timing[i].endT.getMiliSecElapsedFrom(timing[i].startT);
}

AvarageRunInfo GetAvgRunInfo(const TaskRunInfo& runInfo) {
    AvarageRunInfo avgRunInfo;

    if(runInfo.timing.size() <= 2) {
        avgRunInfo.minFPS = 0.f;
        avgRunInfo.maxFPS = 0.f;
        avgRunInfo.avgFPS = 0.f;
        avgRunInfo.minRunTime = 0.f;
        avgRunInfo.maxRunTime = 0.f;
        avgRunInfo.avgRunTime = 0.f;
        return avgRunInfo;
    }

    avgRunInfo.minFPS = std::numeric_limits<float>::max();
    avgRunInfo.maxFPS = std::numeric_limits<float>::min();
    avgRunInfo.avgFPS = 0.f;

    avgRunInfo.minRunTime = std::numeric_limits<float>::max();
    avgRunInfo.maxRunTime = std::numeric_limits<float>::min();
    avgRunInfo.avgRunTime = 0.f;

    size_t sz = runInfo.timing.size();
    for(size_t i = 1; i < sz; ++i) {
        float fpsVal = 1000.f / std::max(0.1f, GetRunTimeBetween(runInfo.timing, i));
        avgRunInfo.minFPS = std::min(fpsVal, avgRunInfo.minFPS);
        avgRunInfo.maxFPS = std::max(fpsVal, avgRunInfo.maxFPS);
        avgRunInfo.avgFPS += fpsVal;

        float runTime = GetRunDurationAt(runInfo.timing, i);
        avgRunInfo.minRunTime = std::min(runTime, avgRunInfo.minRunTime);
        avgRunInfo.maxRunTime = std::max(runTime, avgRunInfo.maxRunTime);
        avgRunInfo.avgRunTime += runTime;
    }

    avgRunInfo.avgFPS /= (sz - 1);
    avgRunInfo.avgRunTime /= sz;

    return avgRunInfo;
}