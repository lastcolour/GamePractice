#include "JobTree.hpp"

#include <cassert>
#include <algorithm>

JobTree::JobTree() :
    pendingJobsCount(0),
    runDelay(0),
    jobsCount(0) {
}

JobTree::~JobTree() {
}

bool JobTree::tryFinishTreeByOneJob() {
    auto remaimingJobs = pendingJobsCount.fetch_sub(1) - 1;
    if(!remaimingJobs) {
        prevTickT = TimePoint::GetNowTime();
        pendingJobsCount.store(jobsCount);
        return true;
    }
    return false;
}

std::vector<ThreadJob*>& JobTree::getRootJobs() {
    return rootJobs;
}

void JobTree::addRootJob(ThreadJob* job) {
    rootJobs.push_back(job);
}

int JobTree::getJobsCount() const {
    return jobsCount;
}

void JobTree::setJobsCount(int newJobsCount) {
    jobsCount = newJobsCount;
    pendingJobsCount.store(jobsCount);
}

bool JobTree::isDelayPassed(const TimePoint& currTime) const {
    return getRemainingWaitTime(currTime).count() == 0;
}

void JobTree::setRunFrequency(int frequency) {
    auto microSecDelay = static_cast<int>((1000000.0 / (static_cast<double>(frequency))));
    runDelay = std::chrono::microseconds(microSecDelay);
}

std::chrono::microseconds JobTree::getRemainingWaitTime(const TimePoint& currTime) const {
    if(prevTickT.getStdTimePoint().time_since_epoch().count() == 0) {
        return std::chrono::microseconds(0);
    }
    auto delta = (prevTickT.getStdTimePoint() + runDelay) - currTime.getStdTimePoint();
    auto microSecVal = std::chrono::duration_cast<std::chrono::microseconds>(delta);
    if(microSecVal.count() < 0) {
        return std::chrono::microseconds(0);
    }
    return microSecVal;
}