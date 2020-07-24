#ifndef __TASKS_RUNNER_HPP__
#define __TASKS_RUNNER_HPP__

#include <vector>
#include <memory>
#include <mutex>
#include <functional>

class RunTask;
class ThreadsPool;
class ThreadJob;

class TasksRunner {
public:

    using PredicateT = std::function<bool(void)>;

public:

    TasksRunner();
    ~TasksRunner();

    void addTask(const RunTask& task);
    void runUntil(int threadCount, PredicateT predicate);

    bool canRun() const;
    RunTask* finishAndGetNext(RunTask* prevTask, int threadId);
    std::vector<RunTask>& getTasks();

private:

    std::mutex mutex;
    PredicateT predFunc;
    std::unique_ptr<ThreadsPool> threadsPool;
    std::vector<RunTask> tasks;
};

#endif /* __TASKS_RUNNER_HPP__ */