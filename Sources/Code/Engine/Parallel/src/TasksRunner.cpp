#include "Parallel/TasksRunner.hpp"
#include "Parallel/RunTask.hpp"
#include "ThreadsPool.hpp"
#include "JobTree.hpp"

#include <cassert>
#include <unordered_map>
#include <limits>
#include <thread>
#include <algorithm>
#include <cstdlib>

namespace {

void mergeJobTrees(JobTree* tree, JobTree* otherTree, ThreadJob* ignoreRootJob) {
    std::vector<ThreadJob*> otherJobs;
    for(auto job : otherTree->getRootJobs()) {
        if(job != ignoreRootJob) {
            tree->addRootJob(job);
        }
        otherJobs.push_back(job);
    }
    int mergedJobs = 0;
    while(!otherJobs.empty()) {
        auto job = otherJobs.back();
        otherJobs.pop_back();
        if(job->getTree() != tree) {
            job->getTree()->setJobsCount(0);
            job->setTree(tree);
            ++mergedJobs;
            for(auto childJob : job->getChildJobs()) {
                otherJobs.push_back(childJob);
            }
        }
    }
    tree->setJobsCount(tree->getJobsCount() + mergedJobs);
}

void calculateParents(JobTree* tree) {
    std::vector<ThreadJob*> queue;
    for(auto job : tree->getRootJobs()) {
        queue.push_back(job);
    }
    int treeMinFrequency = std::numeric_limits<int>::max();
    bool trackPerformance = false;
    while(!queue.empty()) {
        auto job = queue.back();
        queue.pop_back();

        treeMinFrequency = std::min(treeMinFrequency, job->getTask()->getFrequency());
        trackPerformance |= job->getTask()->getTrackPerformance();

        for(auto childJob : job->getChildJobs()) {
            childJob->setParentsCount(childJob->getParentsCount() + 1);
            queue.push_back(childJob);
        }
    }
    if(treeMinFrequency > 0) {
        tree->setRunFrequency(treeMinFrequency);
        tree->setTrackPerformance(trackPerformance);
    }
}

} // namespace

TasksRunner::TasksRunner() :
    predicateFailed(false),
    mode(RunMode::None) {
}

TasksRunner::~TasksRunner() {
}

RunTask* TasksRunner::createTask(const char* name, RunTask::CallT func) {
    assert(mode == RunMode::None && "Invalid run mode");
    auto& task = tasks.emplace_back(new RunTask(name, func));
    return task.get();
}

void TasksRunner::initJobs() {
    std::unordered_map<void*, size_t> taskIdMap;
    for(size_t i = 0u, sz = tasks.size(); i < sz; ++i) {
        auto task = tasks[i].get();
        taskIdMap[task] = i;
        std::unique_ptr<ThreadJob> job(new ThreadJob(task));
        jobs.push_back(std::move(job));
    }
    for(size_t i = 0u, sz = tasks.size(); i < sz; ++i) {
        auto parentJob = jobs[i].get();
        auto parentTask =  tasks[i].get();
        for(auto& childTask : parentTask->getChildren()) {
            auto childIdx = taskIdMap[childTask];
            auto childJob = jobs[childIdx].get();
            parentJob->addChildJob(childJob);
        }
    }
}

void TasksRunner::initJobTrees() {
    std::unordered_map<void*, size_t> jobIdMap;
    for(size_t i = 0u, sz = jobs.size(); i < sz; ++i) {
        auto job = jobs[i].get();
        jobIdMap[job] = i;
        jobTrees.emplace_back(new JobTree(static_cast<int>(i)));
        jobTrees[i]->setJobsCount(1);
        jobTrees[i]->addRootJob(job);
        job->setTree(jobTrees[i].get());
    }

    for(size_t i = 0u, sz = jobs.size(); i < sz; ++i) {
        if(jobTrees[i]->getJobsCount() == 0) {
            continue;
        }
        auto rootJob = jobs[i].get();
        auto rootTree = rootJob->getTree();

        std::vector<size_t> queue;

        queue.push_back(i);
        while(!queue.empty()) {
            auto idx = queue.front();
            std::swap(queue.front(), queue.back());
            queue.pop_back();

            auto job = jobs[idx].get();
            for(auto childJob : job->getChildJobs()) {
                auto childIdx = jobIdMap[childJob];
                if(childJob->getTree() != rootTree) {
                    mergeJobTrees(rootTree, childJob->getTree(), childJob);
                }
                queue.push_back(childIdx);
            }
        }
    }

    jobTrees.erase(std::remove_if(jobTrees.begin(), jobTrees.end(), [](const std::unique_ptr<JobTree>& jobTree){
        return jobTree->getJobsCount() == 0;
    }), jobTrees.end());

    assert(!jobTrees.empty() && "Can't generate any job tree");

    for(auto& jobTree : jobTrees) {
        calculateParents(jobTree.get());
        auto& treeRootJobs = jobTree->getRootJobs();
        pendingJobs.insert(pendingJobs.end(), treeRootJobs.begin(), treeRootJobs.end());
    }
}

void TasksRunner::runUntil(int threadCount, TasksRunner::PredicateT predicate) {
    assert(mode == RunMode::None && "Invalid run mode");
    mode = RunMode::RunUntil;
    if(tasks.empty()) {
        return;
    }
    predFunc = predicate;
    if(!predFunc()) {
        return;
    }
    initJobs();
    initJobTrees();
    threadsPool.reset(new ThreadsPool(this));
    threadsPool->run(threadCount);
    threadsPool.reset();
}

bool TasksRunner::canRun() const {
    return !predicateFailed.load();
}

ThreadJob* TasksRunner::finishAndGetNext(ThreadJob* prevJob, int threadId) {
    if(threadId == 0) {
        auto res = predFunc();
        predicateFailed.store(!res);
        if(!res) {
            return nullptr;
        }
    } else if(predicateFailed.load()) {
        return nullptr;
    }

    ThreadJob* nextJob = nullptr;
    {
        std::unique_lock<std::mutex> ulock(mutex);
        if(prevJob) {
            prevJob->scheduleNextJobs(pendingJobs);
        }
        while(!predicateFailed.load()) {
            auto currTime = TimePoint::GetNowTime();
            nextJob = getNextJob(currTime, threadId);
            if(nextJob) {
                break;
            }
            ulock.unlock();
            std::this_thread::yield();
            ulock.lock();
        }
    }
    return nextJob;
}

ThreadJob* TasksRunner::getNextJob(const TimePoint& currTime, int threadId) {
    for(auto it = pendingJobs.begin(), end = pendingJobs.end(); it != end; ++it) {
        auto job = *it;
        if(job->canStartInThread(threadId)) {
            auto jobRemainingWaitTime = job->getRemainingWaitTime(currTime);
            if(jobRemainingWaitTime.count() == 0) {
                pendingJobs.erase(it);
                return job;
            }
        }
    }
    return nullptr;
}

std::vector<std::unique_ptr<RunTask>>& TasksRunner::getTasks() {
    return tasks;
}

void TasksRunner::startOtherThreads(int threadCount) {
    assert(mode == RunMode::None && "Invlaid run mode");
    assert(!predFunc && "Invalid run predicate");
    predFunc = [](){ return true; };
    mode = RunMode::MainThreadManualStep;
    if(tasks.empty()) {
        return;
    }
    initJobs();
    initJobTrees();
    threadsPool.reset(new ThreadsPool(this));
    threadsPool->startOtherThreads(threadCount);
}

void TasksRunner::stepMainTread() {
    assert(mode == RunMode::MainThreadManualStep && "Invlaid run mode");
    threadsPool->stepMainThread();
}

void TasksRunner::stopOtherTreads() {
    assert(mode == RunMode::MainThreadManualStep && "Invlaid run mode");
    predicateFailed.store(true);
    threadsPool->stopOtherTreads();
}