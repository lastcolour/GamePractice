#ifndef __THREAD_JOB_HPP__
#define __THREAD_JOB_HPP__

class RunTask;

class ThreadJob {
public:

    ThreadJob(RunTask* runTask);
    ThreadJob(const ThreadJob& other);
    ThreadJob& operator=(const ThreadJob& other);
    ~ThreadJob();

    void execute();

public:

    RunTask* task;
};

#endif /* __THREAD_JOB_HPP__ */