#ifndef __TASK_RUN_INFO_HPP__
#define __TASK_RUN_INFO_HPP__

#include "Core/TimePoint.hpp"
#include "Core/CycleArray.hpp"

struct AvarageRunInfo {
    float minFPS;
    float maxFPS;
    float avgFPS;

    float minRunTime;
    float maxRunTime;
    float avgRunTime;
};

class TaskRunInfo {
public:

    struct StartEndTime {
        TimePoint startT;
        TimePoint endT;
    };

public:

    TaskRunInfo();
    ~TaskRunInfo();

public:

    CycleArray<StartEndTime> timing;
    int runCount;
};

float GetRunTimeBetween(const CycleArray<TaskRunInfo::StartEndTime>& timing, size_t i);

float GetRunDurationAt(const CycleArray<TaskRunInfo::StartEndTime>& timing, size_t i);

AvarageRunInfo GetAvgRunInfo(const TaskRunInfo& runInfo);

#endif /* __TASK_RUN_INFO_HPP__ */