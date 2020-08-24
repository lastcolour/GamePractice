#include "Parallel/RunTask.hpp"

#include <algorithm>
#include <cassert>

RunTask::RunTask(const char* taskName, RunTask::CallT callFunc) :
    name(taskName),
    func(callFunc),
    type(RunTaskType::Default),
    runCount(0),
    frequency(0) {
}

RunTask::~RunTask() {
}

void RunTask::execute() {
    func();
    ++runCount;
}

void RunTask::setType(RunTaskType newType) {
    type = newType;
}

RunTaskType RunTask::getType() const {
    return type;
}

void RunTask::addChild(RunTask* other) {
    childrenTasks.push_back(other);
}

std::vector<RunTask*>& RunTask::getChildren() {
    return childrenTasks;
}

int RunTask::getRunCount() const {
    return runCount;
}

void RunTask::setFrequency(int newFrequency) {
    frequency = newFrequency;
}

int RunTask::getFrequency() const {
    return frequency;
}