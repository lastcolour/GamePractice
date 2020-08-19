#include "JobTree.hpp"

#include <cassert>

JobTree::JobTree() :
    pendingJobsCount(0),
    jobsCount(0),
    runCount(0) {
}

JobTree::~JobTree() {
}

bool JobTree::tryFinishTreeByOneJob() {
    auto remaimingJobs = pendingJobsCount.fetch_sub(1) - 1;
    if(!remaimingJobs) {
        ++runCount;
        pendingJobsCount.store(jobsCount);
    }
    return remaimingJobs == 0;
}

int JobTree::getRunCount() const {
    return runCount;
}

std::vector<ThreadJob*>& JobTree::getRootJobs() {
    return rootJobs;
}

void JobTree::addRootJob(ThreadJob* job) {
    rootJobs.push_back(job);
}

int JobTree::getJobsCount() const {
    return jobsCount;
}

void JobTree::setJobsCount(int newJobsCount) {
    jobsCount = newJobsCount;
    pendingJobsCount.fetch_add(jobsCount);
}