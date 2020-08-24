#ifndef __THREAD_WORKER_HPP__
#define __THREAD_WORKER_HPP__

#include <atomic>
#include <thread>

class ThreadsPool;
class ThreadJob;

class ThreadWorker {
public:

    ThreadWorker(ThreadsPool* parentPool, int threadId);
    ~ThreadWorker();

    void start();
    void stop();
    bool isTerminated() const;

private:

    ThreadJob* getNextJob();

    void run();

private:

    ThreadsPool* pool;
    std::thread thread;
    std::atomic<bool> stopped;
    std::atomic<bool> terminated;
    int id;
};

#endif /* __THREAD_WORKER_HPP__ */