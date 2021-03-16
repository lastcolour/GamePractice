#ifndef __THREAD_POOL_HPP__
#define __THREAD_POOL_HPP__

#include "ThreadJob.hpp"

class TasksRunner;
class ThreadWorker;

class ThreadsPool {
public:

    ThreadsPool(TasksRunner* tasksProvider);
    ~ThreadsPool();

    void run(int numThreads);

    void startOtherThreads(int numThread);
    bool stepMainThread();
    void stopOtherTreads();

    ThreadJob* getNextJobForThread(ThreadJob* prevJob, int threadId);

private:

    bool initWorkers(int numWorkers);
    void deinitWorkers();

private:

    TasksRunner* provider;
    ThreadJob* prevJob;
    std::vector<std::unique_ptr<ThreadWorker>> threadWorkers;
};

#endif /* __THREAD_POOL_HPP__ */