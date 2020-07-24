#include "Parallel/TasksRunner.hpp"
#include "Parallel/RunTask.hpp"
#include "ThreadsPool.hpp"

#include <cassert>
#include <limits>

TasksRunner::TasksRunner() {
}

TasksRunner::~TasksRunner() {
}

void TasksRunner::addTask(const RunTask& task) {
    tasks.push_back(task);
}

void TasksRunner::runUntil(int threadCount, TasksRunner::PredicateT predicate) {
    if(tasks.empty()) {
        return;
    }
    predFunc = predicate;
    if(!predFunc()) {
        return;
    }
    threadsPool.reset(new ThreadsPool(this));
    threadsPool->run(threadCount);
    threadsPool.reset();
}

bool TasksRunner::canRun() const {
    if(!predFunc) {
        return false;
    }
    return predFunc();
}

RunTask* TasksRunner::finishAndGetNext(RunTask* prevTask, int threadId) {
    RunTask* nextTask = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex);
        if(prevTask) {
            prevTask->onFinished();
        }
        if(!predFunc()) {
            return nullptr;
        }
        auto minRunCount = std::numeric_limits<int>::max();
        for(auto& task : tasks) {
            if(!task.canStart(threadId)) {
                continue;
            }
            if(task.getRunCount() < minRunCount) {
                nextTask = &task;
                minRunCount = task.getRunCount();
            }
        }
        if(nextTask) {
            nextTask->onStarted();
        }
    }
    return nextTask;
}

std::vector<RunTask>& TasksRunner::getTasks() {
    return tasks;
}
