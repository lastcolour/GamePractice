#include "ThreadJob.hpp"
#include "Parallel/RunTask.hpp"

#include <cassert>

ThreadJob::ThreadJob(RunTask* runTask) :
    pendingCount(0),
    task(runTask),
    runDelay(0) {

    assert(runTask && "Invalid run task");

    auto microSecDelay = static_cast<int>((1000000.0 / (static_cast<double>(runTask->getFrequency()))));
    runDelay = std::chrono::microseconds(microSecDelay);
}

ThreadJob::~ThreadJob() {
}

void ThreadJob::execute() {
    float dt = currStartT.getSecElapsedFrom(prevStartT);
    prevStartT = currStartT;
    task->execute(dt);
    onFinished();
}

void ThreadJob::onFinished() {
    prevEndT = TimePoint::GetNowTime();
    runStats.onExecuted(prevStartT, prevEndT);
    for(auto job : childJobs) {
        auto val = job->pendingCount.fetch_sub(1);
        assert(val > 0 && "Invalind pendings count");
    }
    pendingCount.store(static_cast<int>(childJobs.size()));
}

RunTask* ThreadJob::getTask() {
    return task;
}

bool ThreadJob::tryStartAt(int threadId, const TimePoint& currTime) {
    if(pendingCount.load() > 0) {
        return false;
    }
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
    if(prevStartT.getStdTimePoint().time_since_epoch().count() == 0) {
        currStartT = currTime;
        for(auto job : childJobs) {
            job->pendingCount.store(static_cast<int>(job->childJobs.size()));
        }
        return true;
    }
    auto delta = currTime.getStdTimePoint() - prevStartT.getStdTimePoint();
    if(delta < runDelay) {
        return false;
    }
    currStartT = currTime;
    return true;
}

const JobRunStats& ThreadJob::getRunStats() const {
    return runStats;
}