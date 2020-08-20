#ifndef __TASKS_RUNNER_HPP__
#define __TASKS_RUNNER_HPP__

#include <vector>
#include <memory>
#include <mutex>
#include <functional>
#include <atomic>

class RunTask;
class ThreadsPool;
class ThreadJob;
class JobTree;
class TimePoint;

class TasksRunner {
public:

    using PredicateT = std::function<bool(void)>;

public:

    TasksRunner();
    ~TasksRunner();

    void addTask(RunTask* task);
    void addTask(std::unique_ptr<RunTask>&& task);
    void runUntil(int threadCount, PredicateT predicate);

    bool canRun() const;
    ThreadJob* TasksRunner::finishAndGetNext(ThreadJob* prevJob, const TimePoint& timePoint, int threadId);
    std::vector<std::unique_ptr<RunTask>>& getTasks();

private:

    void initJobs();
    void initJobTrees();
    void finishTasks();

private:

    std::mutex mutex;
    PredicateT predFunc;
    std::unique_ptr<ThreadsPool> threadsPool;
    std::vector<std::unique_ptr<RunTask>> tasks;
    std::vector<std::unique_ptr<JobTree>> jobTrees;
    std::vector<std::unique_ptr<ThreadJob>> jobs;
    std::vector<ThreadJob*> pendingJobs;
    std::atomic<bool> predicateFailed;
};

#endif /* __TASKS_RUNNER_HPP__ */