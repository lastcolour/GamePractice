#include "Parallel/TasksRunner.hpp"
#include "Parallel/RunTask.hpp"
#include "ThreadsPool.hpp"

#include <cassert>
#include <thread>
#include <cstdlib>

TasksRunner::TasksRunner() :
    predicateFailed(false),
    suspended(false),
    currentIdx(0),
    mode(RunMode::None) {
}

TasksRunner::~TasksRunner() {
}

RunTask* TasksRunner::createTask(const char* name, RunTask::CallT func) {
    assert(mode == RunMode::None && "Invalid run mode");
    auto& task = tasks.emplace_back(new RunTask(name, func));
    return task.get();
}

RunTask* TasksRunner::createTask(const char* name, std::function<void(void)> func) {
    assert(mode == RunMode::None && "Invalid run mode");
    auto& task = tasks.emplace_back(new RunTask(name, [func](float){
        func();
    }));
    return task.get();
}

void TasksRunner::initJobs() {
    for(auto& task : tasks) {
        task->getThreadJob()->init();
    }

    for(auto& task : tasks) {
        LogDebug("[TasksRunner::initJobs] Tasks '%s' initial pending count: %d",
            task->getName(), task->getThreadJob()->getPendingCount());
    }
}

void TasksRunner::runUntil(int threadCount, TasksRunner::PredicateT predicate) {
    assert(mode == RunMode::None && "Invalid run mode");
    predFunc = predicate;
    start(threadCount);
}

void TasksRunner::start(int threadCount) {
    mode = RunMode::RunUntil;

    if(tasks.empty()) {
        return;
    }
    if(predFunc) {
        if(!predFunc()) {
            return;
        }
    }
    initJobs();
    threadsPool.reset(new ThreadsPool(this));
    threadsPool->run(threadCount);
    threadsPool.reset();
}

void TasksRunner::stop() {
    assert(predFunc == nullptr && "can't stop if predicated defined");
    predicateFailed.store(true);
}

bool TasksRunner::canRun() const {
    return !predicateFailed.load();
}

void TasksRunner::updateRunFlag(int threadId) {
    if(threadId != 0) {
        return;
    }
    if(!predFunc) {
        return;
    }
    if(predicateFailed.load()) {
        return;
    }
    auto res = predFunc();
    predicateFailed.store(!res);
}

ThreadJob* TasksRunner::finishAndGetNext(ThreadJob* prevJob, int threadId) {
    updateRunFlag(threadId);
    if(predicateFailed.load()) {
        return nullptr;
    }

    if(suspended.load()) {
        std::unique_lock<std::mutex> lock(mutex);
        cond.wait(lock, [this](){
            return !suspended.load();
        });
    }

    ThreadJob* nextJob = nullptr;
    {
        const int offset = currentIdx.load();
        while(!predicateFailed.load()) {
            auto currTime = TimePoint::GetNow();
            for(size_t i = 0, sz = tasks.size(); i < sz; ++i) {
                const int j = (offset + i) % sz;
                auto& task = tasks[j];
                auto job = task->getThreadJob();
                if(job->tryStartAt(threadId, currTime)) {
                    nextJob = job;
                    currentIdx.store((j + 1) % sz);
                    break;
                }
            }
            if(nextJob) {
                break;
            }
            updateRunFlag(threadId);
            if(threadId != 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            } else {
                std::this_thread::yield();
            }
        }
    }
    return nextJob;
}

void TasksRunner::suspend(bool flag) {
    bool isSuspended = suspended.load();
    if(isSuspended == flag) {
        return;
    }
    if(flag) {
        LogInfo("[TasksRunner::suspend] Suspend tasks execution");
    } else {
        LogInfo("[TasksRunner::suspend] Resume tasks execution");
    }
    suspended.store(flag);
    if(!flag) {
        cond.notify_all();
    }
}

bool TasksRunner::isSuspended() const {
    return suspended.load();
}

std::vector<std::unique_ptr<RunTask>>& TasksRunner::getTasks() {
    return tasks;
}

void TasksRunner::startOtherThreads(int threadCount) {
    assert(mode == RunMode::None && "Invlaid run mode");
    mode = RunMode::MainThreadManualStep;
    if(tasks.empty()) {
        return;
    }
    initJobs();
    threadsPool.reset(new ThreadsPool(this));
    threadsPool->startOtherThreads(threadCount);
}

void TasksRunner::stepMainThread() {
    assert(mode == RunMode::MainThreadManualStep && "Invlaid run mode");
    threadsPool->stepMainThread();
}

void TasksRunner::stopOtherTreads() {
    assert(mode == RunMode::MainThreadManualStep && "Invlaid run mode");
    predicateFailed.store(true);
    threadsPool->stopOtherTreads();
}

bool TasksRunner::getTaskRunInfo(const std::string& taskName, TaskRunInfo& outInfo) {
    for(auto& task : tasks) {
        if(task->getName() == taskName) {
            auto& runStats = task->getThreadJob()->getRunStats();
            runStats.getRunInfo(outInfo);
            return true;
        }
    }
    return false;
}

bool TasksRunner::getTaskRunInfo(const RunTask& queryTask, TaskRunInfo& outInfo) {
    for(auto& task : tasks) {
        if(task.get() == &queryTask) {
            auto& runStats = task->getThreadJob()->getRunStats();
            runStats.getRunInfo(outInfo);
            return true;
        }
    }
    return false;
}