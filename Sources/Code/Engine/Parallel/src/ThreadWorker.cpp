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
        auto job = pool->getNextJobForThread(prevJob, id);
        if(job) {
            job->execute();
        } else {
            std::this_thread::yield();
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