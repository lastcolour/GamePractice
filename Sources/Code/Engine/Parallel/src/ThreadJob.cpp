#include "ThreadJob.hpp"
#include "Parallel/RunTask.hpp"

#include <cassert>

ThreadJob::ThreadJob(RunTask* runTask) :
    task(runTask) {
}

ThreadJob::ThreadJob(const ThreadJob& other) :
    task(other.task) {
}

ThreadJob& ThreadJob::operator=(const ThreadJob& other) {
    task = other.task;
    return *this;
}

ThreadJob::~ThreadJob() {
}

void ThreadJob::execute() {
    assert(task && "Invalid task");
    task->execute();
}