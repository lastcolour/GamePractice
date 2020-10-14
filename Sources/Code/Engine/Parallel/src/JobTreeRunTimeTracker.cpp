#include "JobTreeRunTimeTracker.hpp"
#include "JobTree.hpp"
#include "ThreadJob.hpp"
#include "Parallel/RunTask.hpp"

#include "Core/ETLogger.hpp"

#include <cassert>

namespace {

const float LOG_DELAY_SEC = 5.f;

} // namespace

JobTreeRunTimeTracker::JobTreeRunTimeTracker(JobTree* tree) :
    jobTree(tree),
    overrunCount(0) {
    assert(jobTree && "Invalid job tree");
}

JobTreeRunTimeTracker::~JobTreeRunTimeTracker() {
}

void JobTreeRunTimeTracker::collectData() {
    std::vector<ThreadJob*> jobs = jobTree->getRootJobs();
    int i = 0;
    while(!jobs.empty()) {
        ++i;
        auto job = jobs.back();
        float jobRunTime = job->getLastRunTime().count() / 1000.f;
        float jobQueueTime = job->getLastWaitTime().count() / 1000.f;
        if(i > tasksData.size()) {
            TaskOverrunRecord record;
            record.task = job->getTask();
            record.times.push_back({jobRunTime, jobQueueTime});
            tasksData.push_back(record);
        } else {
            auto& record = tasksData[i - 1];
            assert(record.task == job->getTask() && "Invalid run task");
            record.times.push_back({jobRunTime, jobQueueTime});
        }
        jobs.pop_back();
        for(auto childJob : job->getChildJobs()) {
            jobs.push_back(childJob);
        }
    }
}

void JobTreeRunTimeTracker::onTreeFinished() {
    auto runDelay = jobTree->getRunDelay();
    if(runDelay.count() == 0) {
        return;
    }

    auto startT = jobTree->getStartTime();
    auto endT = jobTree->getEndTime();

    auto runTime = std::chrono::duration_cast<std::chrono::microseconds>(
        endT.getStdTimePoint() - startT.getStdTimePoint());

    if(runTime > runDelay) {
        overrunCount += 1;
        collectData();
    }

    if(overrunCount == 0) {
        return;
    }

    if(endT.getSecElapsedFrom(lastLogTime) < LOG_DELAY_SEC) {
        return;
    }

    LogWarning("[JobTreeRunTimeTracker::onTreeFinished] Tree <%d> overruns %d times over last '%.1f' sec; (Expected: '%.1f ms')",
        jobTree->getId(), overrunCount, LOG_DELAY_SEC, runDelay.count() / 1000.f);

    for(auto& record : tasksData) {
        float avgRunTime = 0.f;
        float avgQueueTime = 0.f;
        for(size_t i = 0, sz = record.times.size(); i < sz; ++i) {
            avgRunTime += record.times[i].first;
            avgQueueTime += record.times[i].second;
        }
        avgRunTime /= record.times.size();
        avgQueueTime /= record.times.size();
        LogWarning("[JobTree::checkRunTime]  * <%d> Task '%s' - avg run: '%.1f ms'; avg queue: '%.1f ms'",
            jobTree->getId(), record.task->getName(), avgRunTime, avgQueueTime);
    }

    lastLogTime = endT;
    overrunCount = 0;
    tasksData.clear();
}