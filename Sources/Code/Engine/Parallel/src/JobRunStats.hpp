#ifndef __JOB_RUN_STATS_HPP__
#define __JOB_RUN_STATS_HPP__

#include "Parallel/TaskRunInfo.hpp"

class JobRunStats {
public:

    JobRunStats();
    ~JobRunStats();

    void onExecuted(const TimePoint& startT, const TimePoint& endT);
    void getRunInfo(TaskRunInfo& outInfo) const;

private:

    mutable std::mutex mutex;
    TaskRunInfo runInfo;
};

#endif /* __JOB_RUN_STATS_HPP__ */