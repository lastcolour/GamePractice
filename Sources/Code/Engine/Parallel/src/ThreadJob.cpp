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
    auto now = TimePoint::GetNowTime();
    assert(tree->isDelayPassed(now) && "Restart tree too early");
    task->execute();
    onFinished();
}

bool ThreadJob::canStartInThread(int threadId) const {
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
    return true;
}

void ThreadJob::onFinished() {
    pendingParents.store(parentsCount);
    if(tree->tryFinishTreeByOneJob()) {
        nextJobs = &(tree->getRootJobs());
    } else {
        nextJobs = &childrenJobs;
        for(auto child : childrenJobs) {
            child->onParentTaskFinished();
        }
    }
}

void ThreadJob::scheduleNextJobs(std::vector<ThreadJob*>& output) {
    if(!nextJobs) {
        output.push_back(this);
        return;
    }
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

std::chrono::microseconds ThreadJob::getRemainingWaitTime(const TimePoint& currTime) const {
    return tree->getRemainingWaitTime(currTime);
}