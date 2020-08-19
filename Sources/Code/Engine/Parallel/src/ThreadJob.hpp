#ifndef __THREAD_JOB_HPP__
#define __THREAD_JOB_HPP__

#include <atomic>
#include <vector>

class RunTask;
class JobTree;

class ThreadJob {
public:

    ThreadJob(RunTask* runTask);
    ~ThreadJob();

    bool canStart(int threadId) const;
    std::vector<ThreadJob*>& getNextJob();
    std::vector<ThreadJob*>& getChildJobs();
    void setTree(JobTree* tree);
    JobTree* getTree();
    void setParentsCount(int newParentCount);
    int getParentsCount() const;
    void execute();
    int getRunCount() const;
    void addChildJob(ThreadJob* childJob);
    RunTask* getTask();

private:

    void onFinished();
    void onParentTaskFinished();

private:

    std::vector<ThreadJob*> childrenJobs;
    std::vector<ThreadJob*>* nextJobs;
    std::atomic<int> pendingParents;
    RunTask* task;
    JobTree* tree;
    int parentsCount;
};

#endif /* __THREAD_JOB_HPP__ */