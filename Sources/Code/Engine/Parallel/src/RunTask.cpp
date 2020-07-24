#include "Parallel/RunTask.hpp"

RunTask::RunTask(RunTask::CallT callFunc) :
    func(callFunc),
    type(RunTaskType::Default),
    state(RunState::Waiting),
    runCount(0) {
}

RunTask::RunTask(const RunTask& other) :
    func(other.func),
    type(other.type),
    state(other.state),
    runCount(other.runCount) {
}

RunTask& RunTask::operator=(const RunTask& other) {
    func = other.func;
    type = other.type;
    state = other.state;
    runCount = other.runCount;
    return *this;
}

RunTask::~RunTask() {
}

void RunTask::execute() {
    func();
}

void RunTask::setType(RunTaskType newType) {
    type = newType;
}

int RunTask::getRunCount() const {
    return runCount;
}

bool RunTask::canStart(int threadId) const {
    return state == RunState::Waiting;
}

void RunTask::onStarted() {
    state = RunState::Running;
}

void RunTask::onFinished() {
    state = RunState::Waiting;
    ++runCount;
}
