#ifndef __THREAD_JOB_HPP__
#define __THREAD_JOB_HPP__

#include <atomic>

#include "JobRunStats.hpp"

class RunTask;

class ThreadJob {
public:

    ThreadJob(RunTask* runTask);
    ~ThreadJob();

    bool tryStartAt(int threadId, const TimePoint& currTime);
    void execute();
    RunTask* getTask();
    const JobRunStats& getRunStats() const;

private:

    void onFinished();

private:

    JobRunStats runStats;
    TimePoint prevStartT;
    TimePoint prevEndT;
    TimePoint currStartT;
    std::vector<ThreadJob*> childJobs;
    std::atomic<int> pendingCount;
    RunTask* task;
    std::chrono::microseconds runDelay;
};

#endif /* __THREAD_JOB_HPP__ */