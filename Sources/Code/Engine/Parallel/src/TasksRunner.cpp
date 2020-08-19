#include "Parallel/TasksRunner.hpp"
#include "Parallel/RunTask.hpp"
#include "ThreadsPool.hpp"
#include "JobTree.hpp"

#include <cassert>
#include <unordered_map>
#include <limits>
#include <algorithm>

namespace {

void mergeJobTrees(JobTree* tree, JobTree* otherTree, ThreadJob* ignoreRootJob) {
    tree->setJobsCount(tree->getJobsCount() + otherTree->getJobsCount());

    std::vector<ThreadJob*> otherJobs;
    for(auto job : otherTree->getRootJobs()) {
        if(job != ignoreRootJob) {
            tree->addRootJob(job);
        }
        otherJobs.push_back(job);
    }
    while(!otherJobs.empty()) {
        auto job = otherJobs.back();
        otherJobs.pop_back();
        job->setTree(tree);
        for(auto childJob : job->getChildJobs()) {
            otherJobs.push_back(childJob);
        }
    }
    otherTree->setJobsCount(0);
}

} // namespace

TasksRunner::TasksRunner() :
    predicateFailed(false) {
}

TasksRunner::~TasksRunner() {
}

void TasksRunner::addTask(RunTask* task) {
    assert(task && "Invalid run task");
    tasks.emplace_back(task);
}

void TasksRunner::addTask(std::unique_ptr<RunTask>&& task) {
    assert(task && "Invalid run task");
    tasks.push_back(std::move(task));
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
        auto parentTask = tasks[i].get();
        for(auto childTask : parentTask->getChildren()) {
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
        jobTrees.emplace_back(new JobTree());
    }

    std::vector<size_t> visited(jobs.size(), 0);
    for(size_t i = 0u, sz = jobs.size(); i < sz; ++i) {
        if(visited[i]) {
            continue;
        }
        auto rootTree = jobTrees[i].get();
        rootTree->addRootJob(jobs[i].get());

        std::vector<size_t> queue;
        queue.push_back(i);
        while(!queue.empty()) {
            auto idx = queue.front();
            visited[idx] = 1;
            std::swap(queue.front(), queue.back());
            queue.pop_back();

            rootTree->setJobsCount(rootTree->getJobsCount() + 1);

            auto job = jobs[i].get();
            job->setTree(rootTree);

            for(auto childJob : job->getChildJobs()) {
                auto childIdx = jobIdMap[childJob];
                childJob->setParentsCount(childJob->getParentsCount() + 1);
                if(visited[childIdx]) {
                    if(childJob->getTree() != rootTree) {
                        mergeJobTrees(rootTree, childJob->getTree(), childJob);
                    }
                } else {
                    queue.push_back(childIdx);
                }
            }
        }
    }

    jobTrees.erase(std::remove_if(jobTrees.begin(), jobTrees.end(), [](const std::unique_ptr<JobTree>& jobTree){
        return jobTree->getJobsCount() == 0;
    }), jobTrees.end());

    for(auto& jobTree : jobTrees) {
        auto& treeRootJobs = jobTree->getRootJobs();
        pendingJobs.insert(pendingJobs.end(), treeRootJobs.begin(), treeRootJobs.end());
    }
}

void TasksRunner::runUntil(int threadCount, TasksRunner::PredicateT predicate) {
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
    finishTasks();
}

void TasksRunner::finishTasks() {
    for(auto& job : jobs) {
        auto task = job->getTask();
        task->setRunCount(job->getRunCount());
    }
}

bool TasksRunner::canRun() const {
    return !predicateFailed.load();
}

ThreadJob* TasksRunner::finishAndGetNext(ThreadJob* prevJob, int threadId) {
    ThreadJob* nextJob = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex);
        if(threadId == 0) {
            auto res = predFunc();
            predicateFailed.store(!res);
        }
        if(predicateFailed.load()) {
            return nullptr;
        }
        if(prevJob) {
            for(auto job : prevJob->getNextJob()) {
                pendingJobs.push_back(job);
            }
        }
        size_t nextJobIdx = 0;
        auto minRunCount = std::numeric_limits<int>::max();
        for(size_t i = 0u, sz = pendingJobs.size(); i < sz; ++i) {
            auto job = pendingJobs[i];
            if(!job->canStart(threadId)) {
                continue;
            }
            if(job->getRunCount() < minRunCount) {
                nextJobIdx = i;
                nextJob = job;
                minRunCount = nextJob->getRunCount();
            }
        }
        if(nextJob) {
            std::swap(pendingJobs[pendingJobs.size() - 1], pendingJobs[nextJobIdx]);
            pendingJobs.pop_back();
        }
    }
    return nextJob;
}

std::vector<std::unique_ptr<RunTask>>& TasksRunner::getTasks() {
    return tasks;
}
