#include "ThreadWorker.hpp"
#include "ThreadsPool.hpp"
#include "ThreadJob.hpp"
#include "Parallel/TasksRunner.hpp"
#include "Core/TimePoint.hpp"

#include <cassert>
#include <cstdlib>

ThreadsPool::ThreadsPool(TasksRunner* tasksProvider) :
    provider(tasksProvider),
    prevJob(nullptr) {
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
    while(provider->canRun()) {
        if(!stepMainThread()) {
            std::this_thread::yield();
        }
    }
    deinitWorkers();
}

void ThreadsPool::startOtherThreads(int numThreads) {
    assert(numThreads > 0 && "Invalid thread number");
    if(!initWorkers(numThreads)) {
        return;
    }
}

bool ThreadsPool::stepMainThread() {
    auto job = getNextJobForThread(prevJob, 0);
    prevJob = job;
    if(job) {
        job->execute();
        return true;
    }
    return false;
}

void ThreadsPool::stopOtherTreads() {
    deinitWorkers();
}