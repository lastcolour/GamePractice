#ifndef __TASKS_STATS_TRACKER_HPP__
#define __TASKS_STATS_TRACKER_HPP__

#include "Render/ETDebugRender.hpp"
#include "Core/SystemLogic.hpp"

class TasksStatsTracker : public SystemLogic,
    public ETNode<ETDebugInfoProvider> {
public:

    TasksStatsTracker();
    virtual ~TasksStatsTracker();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETDebugInfoProvider
    void ET_drawDebugInfo(DebugInfoDrawer& dd) override;
};

#endif /* __TASKS_STATS_TRACKER_HPP__ */