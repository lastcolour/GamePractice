#ifndef __UI_TIMER_HPP__
#define __UI_TIMER_HPP__

#include "Core/ETPrimitives.hpp"
#include "Core/SystemLogic.hpp"
#include "Core/ETTasks.hpp"

class UITimer : public SystemLogic,
    public ETNode<ETUIUpdateTask> {
public:

    UITimer();
    virtual ~UITimer();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETUIUpdateTask
    void ET_updateUI() override;
};

#endif /* __UI_TIMER_HPP__ */