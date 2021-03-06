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
    prevStartT = TimePoint::GetNowTime();
    tree->tryRestartTree(prevStartT);
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
    prevEndT = TimePoint::GetNowTime();
    pendingParents.store(parentsCount);
    if(tree->tryFinishTreeByOneJob(prevEndT)) {
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
            job->prevScheduleT = prevEndT;
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

std::chrono::microseconds ThreadJob::getLastRunTime() const {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        prevEndT.getStdTimePoint() - prevStartT.getStdTimePoint());
}

std::chrono::microseconds ThreadJob::getLastWaitTime() const {
    if(parentsCount == 0) {
        return std::chrono::microseconds(0);
    } else {
        return std::chrono::duration_cast<std::chrono::microseconds>(
            prevStartT.getStdTimePoint() - prevScheduleT.getStdTimePoint());
    }
}