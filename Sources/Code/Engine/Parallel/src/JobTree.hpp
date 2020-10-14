#ifndef __JOB_TREE_HPP__
#define __JOB_TREE_HPP__

#include "Core/TimePoint.hpp"
#include "JobTreeRunTimeTracker.hpp"

#include <vector>
#include <atomic>

class ThreadJob;

class JobTree {
public:

    JobTree(int jobTreeId);
    ~JobTree();

    void tryRestartTree(const TimePoint& currTime);
    bool tryFinishTreeByOneJob(const TimePoint& currTime);
    void addRootJob(ThreadJob* job);
    std::vector<ThreadJob*>& getRootJobs();
    int getJobsCount() const;
    void setJobsCount(int newJobsCount);
    void setRunFrequency(int frequency);
    std::chrono::microseconds getRemainingWaitTime(const TimePoint& currTime) const;

    const TimePoint& getStartTime() const;
    const TimePoint& getEndTime() const;
    std::chrono::microseconds getRunDelay() const;
    int getId() const;

private:

    int treeId;
    TimePoint prevStartT;
    TimePoint prevEndT;
    std::vector<ThreadJob*> rootJobs;
    std::atomic<int> pendingJobsCount;
    std::chrono::microseconds runDelay;
    int jobsCount;
    std::atomic<bool> isRunning;
    JobTreeRunTimeTracker tracker;
};

#endif /* __JOB_TREE_HPP__ */