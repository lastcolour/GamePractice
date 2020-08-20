#include "Parallel/RunTask.hpp"

#include <algorithm>
#include <cassert>

RunTask::RunTask(const std::string& taskName, RunTask::CallT callFunc) :
    name(taskName),
    func(callFunc),
    type(RunTaskType::Default),
    runCount(0),
    frequency(60) {
}

RunTask::~RunTask() {
}

void RunTask::execute() {
    func();
}

void RunTask::setType(RunTaskType newType) {
    type = newType;
}

RunTaskType RunTask::getType() const {
    return type;
}

void RunTask::addChild(std::unique_ptr<RunTask>& other) {
    assert(other && "Invalid child task");
    auto it = std::find(childrenTasks.begin(), childrenTasks.end(), other.get());
    assert(it == childrenTasks.end() && "Children already exists");
    childrenTasks.push_back(other.get());
}

std::vector<RunTask*>& RunTask::getChildren() {
    return childrenTasks;
}

void RunTask::setRunCount(int newRunCount) {
    runCount = newRunCount;
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