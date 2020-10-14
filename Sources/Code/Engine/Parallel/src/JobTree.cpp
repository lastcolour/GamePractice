#include "JobTree.hpp"
#include "Parallel/RunTask.hpp"
#include "ThreadJob.hpp"

#include <cassert>
#include <algorithm>

JobTree::JobTree(int jobTreeId) :
    treeId(jobTreeId),
    pendingJobsCount(0),
    runDelay(0),
    jobsCount(0),
    isRunning(false),
    tracker(this) {
}

JobTree::~JobTree() {
}

bool JobTree::tryFinishTreeByOneJob(const TimePoint& currTime) {
    auto remaimingJobs = pendingJobsCount.fetch_sub(1) - 1;
    if(!remaimingJobs) {
        prevEndT = currTime;
        isRunning.store(false);
        pendingJobsCount.store(jobsCount);
        tracker.onTreeFinished();
        return true;
    }
    return false;
}

std::vector<ThreadJob*>& JobTree::getRootJobs() {
    return rootJobs;
}

void JobTree::tryRestartTree(const TimePoint& currTime) {
    bool falseVal = false;
    if(!isRunning.compare_exchange_strong(falseVal, true)) {
        return;
    }
    assert(getRemainingWaitTime(currTime).count() == 0 && "Restart tree too early");
    prevStartT = currTime;
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

void JobTree::setRunFrequency(int frequency) {
    auto microSecDelay = static_cast<int>((1000000.0 / (static_cast<double>(frequency))));
    runDelay = std::chrono::microseconds(microSecDelay);
}

std::chrono::microseconds JobTree::getRemainingWaitTime(const TimePoint& currTime) const {
    if(prevStartT.getStdTimePoint().time_since_epoch().count() == 0) {
        return std::chrono::microseconds(0);
    }
    if(isRunning.load()) {
        return std::chrono::microseconds(0);
    }
    auto delta = currTime.getStdTimePoint() - prevStartT.getStdTimePoint();
    auto waitTime = std::chrono::duration_cast<std::chrono::microseconds>(runDelay - delta);
    if(waitTime.count() < 0) {
        return std::chrono::microseconds(0);
    }
    return waitTime;
}

const TimePoint& JobTree::getStartTime() const {
    return prevStartT;
}

const TimePoint& JobTree::getEndTime() const {
    return prevEndT;
}

std::chrono::microseconds JobTree::getRunDelay() const {
    return runDelay;
}

int JobTree::getId() const {
    return treeId;
}