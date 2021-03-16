#include "JobTree.hpp"
#include "Parallel/RunTask.hpp"
#include "ThreadJob.hpp"

#include <cassert>

JobTree::JobTree(int jobTreeId) :
    tracker(this),
    pendingJobsCount(0),
    jobsCount(0),
    treeId(jobTreeId),
    isRunning(false),
    trackPerformance(false) {
}

JobTree::~JobTree() {
}

bool JobTree::tryFinishTreeByOneJob(const TimePoint& currTime) {
    auto remaimingJobs = pendingJobsCount.fetch_sub(1) - 1;
    if(!remaimingJobs) {
        prevEndT = currTime;
        isRunning.store(false);
        pendingJobsCount.store(jobsCount);
        if(trackPerformance) {
            tracker.onTreeFinished();
        }
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
    assert(canStartAt(currTime) && "Restart tree too early");
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

void JobTree::setTrackPerformance(bool flag) {
    trackPerformance = flag;
}

bool JobTree::canStartAt(const TimePoint& currTime) const {
    if(prevStartT.getStdTimePoint().time_since_epoch().count() == 0) {
        return true;
    }
    if(isRunning.load()) {
        return true;
    }
    auto delta = currTime.getStdTimePoint() - prevStartT.getStdTimePoint();
    if(delta >= runDelay) {
        return true;
    }
    return false;
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