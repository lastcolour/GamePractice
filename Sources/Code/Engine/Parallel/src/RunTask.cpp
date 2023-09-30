#include "Parallel/RunTask.hpp"
#include "ThreadJob.hpp"

#include <cassert>

RunTask::RunTask(const char* taskName, RunTask::CallT callFunc) :
    threadJob(new ThreadJob(*this)),
    name(taskName),
    func(callFunc),
    frequency(std::numeric_limits<int>::max()),
    type(RunTaskType::Default) {

    assert(taskName && taskName[0] && "Invalid task name");
    assert(callFunc && "Invalid call func");
}

RunTask::~RunTask() {
}

void RunTask::execute(float dt) {
    func(dt);
}

void RunTask::setType(RunTaskType newType) {
    type = newType;
}

RunTaskType RunTask::getType() const {
    return type;
}

const char* RunTask::getName() const {
    return name.c_str();
}

void RunTask::addChild(RunTask* other) {
    if(!other) {
        return;
    }
    if(other == this) {
        assert(false && "Can't add self as a child task");
        return;
    }
    for(auto child : childrenTasks) {
        if(child == other) {
            assert(false && "Double add of a child");
            return;
        }
    }
    other->threadJob->setPendingCount(other->threadJob->getPendingCount() + 1);
    childrenTasks.push_back(other);
    other->childrenTasks.push_back(this);
}

std::vector<RunTask*>& RunTask::getChildren() {
    return childrenTasks;
}

const std::vector<RunTask*>& RunTask::getChildren() const {
    return childrenTasks;
}

void RunTask::setFrequency(int newFrequency) {
    frequency = newFrequency;
}

int RunTask::getFrequency() const {
    return frequency;
}

ThreadJob* RunTask::getThreadJob() {
    return threadJob.get();
}