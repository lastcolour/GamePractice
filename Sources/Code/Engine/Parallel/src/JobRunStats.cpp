#include "JobRunStats.hpp"

JobRunStats::JobRunStats() {
}

JobRunStats::~JobRunStats() {
}

void JobRunStats::onExecuted(const TimePoint& startT, const TimePoint& endT) {
    std::lock_guard<std::mutex> lock(mutex);
    runInfo.timing.insert(TaskRunInfo::StartEndTime{startT, endT});
    ++runInfo.runCount;
}

void JobRunStats::getRunInfo(TaskRunInfo& outInfo) const {
    std::lock_guard<std::mutex> lock(mutex);
    outInfo = runInfo;
}