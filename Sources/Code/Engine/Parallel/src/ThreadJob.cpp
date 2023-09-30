#include "ThreadJob.hpp"
#include "Parallel/RunTask.hpp"

#include <cassert>

ThreadJob::ThreadJob(RunTask& runTask) :
    task(runTask),
    pendingCount(0),
    runDelay(0),
    childrenCount(0),
    isFirstRun(true) {
}

ThreadJob::~ThreadJob() {
}

void ThreadJob::init() {
    childrenCount = task.getChildren().size();
    auto microSecDelay = static_cast<int>((1000000.0 / (static_cast<double>(task.getFrequency()))));
    runDelay = std::chrono::microseconds(microSecDelay);
}

void ThreadJob::execute() {
    float dt = currStartT.getSecDeltaWith(prevStartT);
    prevStartT = currStartT;
    task.execute(dt);
    onFinished();
}

void ThreadJob::onFinished() {
    runStats.onExecuted(prevStartT, TimePoint::GetNow());
    for(auto& childTask : task.getChildren()) {
        auto childJob = childTask->getThreadJob();
        auto val = childJob->pendingCount.fetch_sub(1);
        assert(val > 0 && "Invalind pendings count");
    }
    auto prevPendingCount = pendingCount.exchange(static_cast<int>(childrenCount));
    assert(prevPendingCount == -1);
}

RunTask* ThreadJob::getTask() {
    return &task;
}

bool ThreadJob::tryStartAt(int threadId, const TimePoint& currTime) {
    switch(task.getType()) {
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
    int numTaskRunning = -1;
    int numExpected = 0;
    if(!pendingCount.compare_exchange_weak(numExpected, numTaskRunning)) {
        return false;
    }
    if(!isFirstRun) {
        auto delta = currTime.getStdTimePoint() - prevStartT.getStdTimePoint();
        if(delta < runDelay) {
            pendingCount.store(numExpected);
            return false;
        }
    } else {
        isFirstRun = false;
        prevStartT = currTime;
        prevStartT.addMicroSecDelta(-runDelay.count());
    }
    currStartT = currTime;
    return true;
}

const JobRunStats& ThreadJob::getRunStats() const {
    return runStats;
}

void ThreadJob::setPendingCount(int newPendingCount) {
    pendingCount.store(newPendingCount);
}

int ThreadJob::getPendingCount() const {
    return pendingCount.load();
}