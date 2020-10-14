#ifndef __JOB_TREE_RUNTIME_TRACKER_HPP__
#define __JOB_TREE_RUNTIME_TRACKER_HPP__

#include "Core/TimePoint.hpp"

#include <vector>

class JobTree;
class RunTask;

class JobTreeRunTimeTracker {
public:

    JobTreeRunTimeTracker(JobTree* tree);
    ~JobTreeRunTimeTracker();

    void onTreeFinished();

private:

    struct TaskOverrunRecord {
        RunTask* task;
        std::vector<std::pair<float, float>> times;
    };

private:

    void collectData();

private:

    JobTree* jobTree;
    TimePoint lastLogTime;
    std::vector<TaskOverrunRecord> tasksData;
    int overrunCount;
};

#endif /* __JOB_TREE_RUNTIME_TRACKER_HPP__ */