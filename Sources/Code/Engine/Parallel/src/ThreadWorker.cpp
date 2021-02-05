#include "ThreadWorker.hpp"
#include "ThreadsPool.hpp"
#include "ThreadJob.hpp"
#include "Core/TimePoint.hpp"

ThreadWorker::ThreadWorker(ThreadsPool* parentPool, int threadId) :
    pool(parentPool),
    id(threadId) {
}

ThreadWorker::~ThreadWorker() {
}

void ThreadWorker::start() {
    terminated.store(false);
    std::thread t1([this](){
        run();
    });
    thread = std::move(t1);
    thread.detach();
}

void ThreadWorker::run() {
    ThreadJob* prevJob = nullptr;
    while(true) {
        auto job = pool->getNextJobForThread(prevJob, id);
        if(job) {
            job->execute();
            prevJob = job;
        } else {
            break;
        }
    }
    terminated.store(true);
}

bool ThreadWorker::isTerminated() const {
    return terminated.load();
}