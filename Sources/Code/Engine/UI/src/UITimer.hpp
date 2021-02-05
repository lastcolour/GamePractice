#ifndef __UI_TIMER_HPP__
#define __UI_TIMER_HPP__

#include "Core/ETPrimitives.hpp"
#include "Core/SystemLogic.hpp"
#include "Core/TimePoint.hpp"
#include "Core/ETTasks.hpp"
#include "UI/ETUITimer.hpp"

class UITimer : public SystemLogic,
    public ETNode<ETUITimer> {
public:

    UITimer();
    virtual ~UITimer();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETUITimer
    void ET_onTick(float dt) override;
    void ET_setScale(float newScale) override;
    void ET_pause() override;
    void ET_resume() override;
    bool ET_isPaused() const override;

private:

    float scale;
    bool skipUpdate;
    bool isPaused;
};


#endif /* __UI_TIMER_HPP__ */