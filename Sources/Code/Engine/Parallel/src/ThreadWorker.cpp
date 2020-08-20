#include "ThreadWorker.hpp"
#include "ThreadsPool.hpp"
#include "ThreadJob.hpp"

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
    while(!stopped.load()) {
        timePoint = TimePoint::GetNowTime();
        auto job = pool->getNextJobForThread(prevJob, timePoint, id);
        if(job) {
            job->execute();
        } else {
            std::this_thread::yield();
        }
        prevJob = job;
    }
    if(prevJob) {
        timePoint = TimePoint::GetNowTime();
        pool->getNextJobForThread(prevJob, timePoint, id);
        prevJob = nullptr;
    }
    terminated.store(true);
}

bool ThreadWorker::isTerminated() const {
    return terminated.load();
}