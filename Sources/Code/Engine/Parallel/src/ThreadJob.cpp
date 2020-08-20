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

bool ThreadJob::canStart(const TimePoint& currTime, int threadId) const {
    switch(task->getType()) {
        case RunTaskType::Default: {
            break;
        }
        case RunTaskType::MainThreadOnly: {
            if(threadId != 0) {
                return false;
            }
            break;
        }
        case RunTaskType::NoInMainThread: {
            if(threadId == 0) {
                return false;
            }
            break;
        }
        default: {
            assert(false && "Invalid job type");
        }
    }
    if(!parentsCount && !tree->isDelayPassed(currTime)) {
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

void ThreadJob::scheduleNextJobs(std::vector<ThreadJob*>& output) {
    for(auto job : *nextJobs) {
        if(job->pendingParents.load() == 0) {
            job->pendingParents.store(job->parentsCount);
            output.push_back(job);
        }
    }
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