#ifndef __JOB_TREE_HPP__
#define __JOB_TREE_HPP__

#include "Core/TimePoint.hpp"

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

private:

    void checkRunTime();

private:

    int treeId;
    TimePoint prevStartT;
    TimePoint prevEndT;
    std::vector<ThreadJob*> rootJobs;
    std::atomic<int> pendingJobsCount;
    std::chrono::microseconds runDelay;
    int jobsCount;
    std::atomic<bool> isRunning;
};

#endif /* __JOB_TREE_HPP__ */