#include "ThreadWorker.hpp"
#include "ThreadsPool.hpp"
#include "ThreadJob.hpp"
#include "Core/TimePoint.hpp"
#include "ParallelUtils.hpp"

ThreadWorker::ThreadWorker(ThreadsPool* parentPool, int threadId) :
    pool(parentPool),
    id(threadId) {
}

ThreadWorker::~ThreadWorker() {
}

void ThreadWorker::start() {
    terminated.store(false);
    stopped.store(false);
    std::thread t1([this](){
        run();
    });
    thread = std::move(t1);
    thread.detach();
}

void ThreadWorker::stop() {
    stopped.store(true);
}

void ThreadWorker::run() {
    ThreadJob* prevJob = nullptr;
    TimePoint timePoint;
    while(!stopped.load()) {
        timePoint = TimePoint::GetNowTime();
        auto job = pool->getNextJobForThread(prevJob, id);
        if(job) {
            job->execute(timePoint);
        } else {
            Parallel::ThreadIdle();
        }
        prevJob = job;
    }
    if(prevJob) {
        pool->getNextJobForThread(prevJob, id);
        prevJob = nullptr;
    }
    terminated.store(true);
}

bool ThreadWorker::isTerminated() const {
    return terminated.load();
}