#ifndef __TASKS_RUNNER_HPP__
#define __TASKS_RUNNER_HPP__

#include "Parallel/RunTask.hpp"
#include "Parallel/TaskRunInfo.hpp"

#include <mutex>
#include <atomic>
#include <condition_variable>

class ThreadsPool;
class ThreadJob;
class TimePoint;

class TasksRunner {
public:

    using PredicateT = std::function<bool(void)>;

public:

    TasksRunner();
    ~TasksRunner();

    RunTask* createTask(const char* name, RunTask::CallT func);
    RunTask* createTask(const char* name, std::function<void(void)> func);
    void runUntil(int threadCount, PredicateT predicate);
    void start(int threadCount);
    void suspend(bool flag);
    bool isSuspended() const;
    void stop();

    void startOtherThreads(int threadCount);
    void stepMainThread();
    void stopOtherTreads();

    bool canRun() const;
    ThreadJob* finishAndGetNext(ThreadJob* prevJob, int threadId);
    std::vector<std::unique_ptr<RunTask>>& getTasks();

    bool getTaskRunInfo(const std::string& taskName, TaskRunInfo& outInfo);
    bool getTaskRunInfo(const RunTask& task, TaskRunInfo& outInfo);

private:

    void updateRunFlag(int threadId);
    void initJobs();
    ThreadJob* getNextJob(const TimePoint& currTime, int threadId);

private:

    enum class RunMode {
        None = 0,
        RunUntil,
        MainThreadManualStep
    };

private:

    std::mutex mutex;
    PredicateT predFunc;
    std::unique_ptr<ThreadsPool> threadsPool;
    std::vector<std::unique_ptr<RunTask>> tasks;
    std::vector<std::unique_ptr<ThreadJob>> jobs;
    std::vector<ThreadJob*> pendingJobs;
    std::atomic<bool> predicateFailed;
    std::atomic<bool> suspended;
    std::condition_variable cond;
    RunMode mode;
};

#endif /* __TASKS_RUNNER_HPP__ */