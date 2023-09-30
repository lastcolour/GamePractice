#ifndef __THREAD_JOB_HPP__
#define __THREAD_JOB_HPP__

#include <atomic>

#include "JobRunStats.hpp"

class RunTask;

class ThreadJob {
public:

    ThreadJob(RunTask& runTask);
    ~ThreadJob();

    void init();
    bool tryStartAt(int threadId, const TimePoint& currTime);
    void execute();
    RunTask* getTask();
    const JobRunStats& getRunStats() const;
    void setPendingCount(int newPendingCount);
    int getPendingCount() const;

private:

    void onFinished();

private:

    RunTask& task;
    JobRunStats runStats;
    TimePoint prevStartT;
    TimePoint currStartT;
    std::atomic<int> pendingCount;
    std::chrono::microseconds runDelay;
    int childrenCount;
    bool isFirstRun;
};

#endif /* __THREAD_JOB_HPP__ */