#ifndef __FRAME_STATS_HPP__
#define __FRAME_STATS_HPP__

#include "Core/TimePoint.hpp"

#include <vector>

class FrameStatsTracker {
public:

    FrameStatsTracker();
    virtual ~FrameStatsTracker();

    void onFrameStart();
    void onFrameEnd();

private:

    TimePoint frameStartT;
    float fpsValue;
};

#endif /* __FRAME_STATS_HPP__ */