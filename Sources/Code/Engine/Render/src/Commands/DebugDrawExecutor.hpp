#ifndef __DEBUG_DRAW_EXECUTOR_HPP__
#define __DEBUG_DRAW_EXECUTOR_HPP__

#include "Commands/DebugCmdCollector.hpp"

class DebugDrawExecutor {
public:

    DebugDrawExecutor();
    virtual ~DebugDrawExecutor();

    bool init();
    void deinit();
    void draw(RenderState& renderState);

private:

    DebugCmdCollector cmdCollector;
};

#endif /* __DEBUG_DRAW_EXECUTOR_HPP__ */