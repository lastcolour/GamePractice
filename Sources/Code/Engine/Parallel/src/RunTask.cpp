#include "Parallel/RunTask.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

RunTask::RunTask(const char* taskName, RunTask::CallT callFunc) :
    name(taskName),
    func(callFunc),
    type(RunTaskType::Default),
    runCount(0),
    frequency(std::numeric_limits<int>::max()),
    trackPerformance(false) {
}

RunTask::~RunTask() {
}

void RunTask::execute(float dt) {
    func(dt);
    ++runCount;
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

void RunTask::setTrackPerformance(bool flag) {
    trackPerformance = flag;
}

bool RunTask::getTrackPerformance() const {
    return trackPerformance;
}