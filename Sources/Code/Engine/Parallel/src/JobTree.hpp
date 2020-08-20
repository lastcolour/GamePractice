#ifndef __JOB_TREE_HPP__
#define __JOB_TREE_HPP__

#include "Core/TimePoint.hpp"

#include <vector>
#include <atomic>

class ThreadJob;

class JobTree {
public:

    JobTree();
    ~JobTree();

    void addRootJob(ThreadJob* job);
    bool tryFinishTreeByOneJob();
    std::vector<ThreadJob*>& getRootJobs();
    int getJobsCount() const;
    void setJobsCount(int newJobsCount);
    int getRunCount() const;
    bool isDelayPassed(const TimePoint& currTime) const;

private:

    TimePoint prevTickT;
    std::vector<ThreadJob*> rootJobs;
    std::atomic<int> pendingJobsCount;
    float runDelay;
    int jobsCount;
    int runCount;
};

#endif /* __JOB_TREE_HPP__ */