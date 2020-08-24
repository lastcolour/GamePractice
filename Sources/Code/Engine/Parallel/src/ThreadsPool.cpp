#include "ThreadWorker.hpp"
#include "ThreadsPool.hpp"
#include "ThreadJob.hpp"
#include "Parallel/TasksRunner.hpp"
#include "Core/TimePoint.hpp"

#include <cassert>

ThreadsPool::ThreadsPool(TasksRunner* tasksProvider) :
    provider(tasksProvider) {
}

ThreadsPool::~ThreadsPool() {
}

bool ThreadsPool::initWorkers(int numWorkers) {
    for(int i = 0; i < numWorkers; ++i) {
        std::unique_ptr<ThreadWorker> worker(new ThreadWorker(this, i + 1));
        worker->start();
        threadWorkers.push_back(std::move(worker));
    }
    return true;
}

void ThreadsPool::deinitWorkers() {
    for(auto& worker : threadWorkers) {
        worker->stop();
    }
    bool hasRunningThread = true;
    while(hasRunningThread) {
        hasRunningThread = false;
        for(auto& workder : threadWorkers) {
            if(!workder->isTerminated()) {
                hasRunningThread = true;
                break;
            }
        }
        if(hasRunningThread) {
            std::this_thread::yield();
        }
    }
    threadWorkers.clear();
}

ThreadJob* ThreadsPool::getNextJobForThread(ThreadJob* prevJob, int threadId) {
    auto nextJob = provider->finishAndGetNext(prevJob, threadId);
    return nextJob;
}

void ThreadsPool::run(int numThreads) {
    assert(numThreads > 0 && "Invalid thread number");
    if(!initWorkers(numThreads - 1)) {
        return;
    }
    ThreadJob* prevJob = nullptr;
    TimePoint timePoint;
    while(true) {
        timePoint = TimePoint::GetNowTime();
        auto job = getNextJobForThread(prevJob, 0);
        if(job) {
            job->execute(timePoint);
        } else {
            if(provider->canRun()) {
                std::this_thread::yield();
            } else {
                break;
            }
        }
        prevJob = job;
    }
    deinitWorkers();
}