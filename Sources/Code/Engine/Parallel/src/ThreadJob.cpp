#include "ThreadJob.hpp"
#include "Parallel/RunTask.hpp"
#include "JobTree.hpp"

#include <cassert>

ThreadJob::ThreadJob(RunTask* runTask) :
    nextJobs(nullptr),
    task(runTask),
    tree(nullptr),
    parentsCount(0) {

    assert(runTask && "Invalid run task");
}

ThreadJob::~ThreadJob() {
}

void ThreadJob::setTree(JobTree* newTree) {
    tree = newTree;
}

JobTree* ThreadJob::getTree() {
    return tree;
}

void ThreadJob::setParentsCount(int newParentsCount) {
    parentsCount = newParentsCount;
    pendingParents.store(parentsCount);
}

int ThreadJob::getParentsCount() const {
    return parentsCount;
}

void ThreadJob::execute() {
    assert(task && "Invalid task");
    task->execute();
    onFinished();
}

int ThreadJob::getRunCount() const {
    return tree->getRunCount();
}

bool ThreadJob::canStart(int threadId) const {
    if(task->getType() == RunTaskType::MainThreadOnly) {
        if(threadId != 0) {
            return false;
        }
    } else if(task->getType() == RunTaskType::NoInMainThread) {
        if(threadId == 0) {
            return false;
        }
    }
    if(pendingParents.load()) {
        return false;
    }
    return true;
}

void ThreadJob::onFinished() {
    pendingParents.store(parentsCount);
    if(tree->tryFinishTreeByOneJob()) {
        nextJobs = &(tree->getRootJobs());
    } else {
        nextJobs = &childrenJobs;
    }
    for(auto child : childrenJobs) {
        child->onParentTaskFinished();
    }
}

std::vector<ThreadJob*>& ThreadJob::getNextJob() {
    return *nextJobs;
}

void ThreadJob::onParentTaskFinished() {
    pendingParents.fetch_sub(1);
}

void ThreadJob::addChildJob(ThreadJob* childJob) {
    childrenJobs.push_back(childJob);
}

std::vector<ThreadJob*>& ThreadJob::getChildJobs() {
    return childrenJobs;
}

RunTask* ThreadJob::getTask() {
    return task;
}