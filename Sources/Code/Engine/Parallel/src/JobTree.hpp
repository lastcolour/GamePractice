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
    void setTrackPerformance(bool flag);
    bool canStartAt(const TimePoint& currTime) const;

    const TimePoint& getStartTime() const;
    const TimePoint& getEndTime() const;
    std::chrono::microseconds getRunDelay() const;
    int getId() const;

private:

    JobTreeRunTimeTracker tracker;
    std::vector<ThreadJob*> rootJobs;
    TimePoint prevStartT;
    TimePoint prevEndT;
    std::chrono::microseconds runDelay;
    std::atomic<int> pendingJobsCount;
    int jobsCount;
    int treeId;
    std::atomic<bool> isRunning;
    bool trackPerformance;
};

#endif /* __JOB_TREE_HPP__ */