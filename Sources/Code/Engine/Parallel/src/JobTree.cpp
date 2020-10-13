#include "JobTree.hpp"
#include "Parallel/RunTask.hpp"
#include "ThreadJob.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>
#include <algorithm>

JobTree::JobTree(int jobTreeId) :
    treeId(jobTreeId),
    pendingJobsCount(0),
    runDelay(0),
    jobsCount(0),
    isRunning(false) {
}

JobTree::~JobTree() {
}

void JobTree::checkRunTime() {
    auto runTime = std::chrono::duration_cast<std::chrono::microseconds>(
        prevEndT.getStdTimePoint() - prevStartT.getStdTimePoint());
    if(runTime <= runDelay) {
        return;
    }

    LogWarning("[JobTree::checkRunTime] Tree <%d> runs too long; (Expected: '%.1f ms' vs actual '%.1f ms')",
        treeId, runDelay.count() / 1000.f, runTime.count() / 1000.f);

    std::vector<ThreadJob*> jobs = rootJobs;
    while(!jobs.empty()) {
        auto job = jobs.back();
        auto jobRunTime = job->getLastRunTime();
        auto jobQueueTime = job->getLastWaitTime();
        LogWarning("[JobTree::checkRunTime]  * <%d> Task '%s' - run time: '%.1f ms'; queue time: '%.1f ms'",
            treeId, job->getTask()->getName(), jobRunTime.count() / 1000.f, jobQueueTime.count() / 1000.f);
        jobs.pop_back();
        for(auto childJob : job->getChildJobs()) {
            jobs.push_back(childJob);
        }
    }
}

bool JobTree::tryFinishTreeByOneJob(const TimePoint& currTime) {
    auto remaimingJobs = pendingJobsCount.fetch_sub(1) - 1;
    if(!remaimingJobs) {
        prevEndT = currTime;
        isRunning.store(false);
        pendingJobsCount.store(jobsCount);
        checkRunTime();
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
    auto delta = (prevStartT.getStdTimePoint() + runDelay) - currTime.getStdTimePoint();
    auto microSecVal = std::chrono::duration_cast<std::chrono::microseconds>(delta);
    if(microSecVal.count() < 0) {
        return std::chrono::microseconds(0);
    }
    return microSecVal;
}