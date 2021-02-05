#ifndef __ANDROIND_EVENT_MANGER_HPP__
#define __ANDROIND_EVENT_MANGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETTasks.hpp"
#include "Core/ETPrimitives.hpp"
#include "Platform/Android/ETAndroidInterfaces.hpp"

class AndroidEventManager : public SystemLogic,
    public ETNode<ETInputUpdateTask>,
    public ETNode<ETAndroidActivityEvents>,
    public ETNode<ETAndroidInputEvents> {
public:

    AndroidEventManager();
    virtual ~AndroidEventManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETInputUpdateTask
    void ET_updateInput() override;

    // ETAndroidActivityEvents
    void ET_onActivityEvent(ActivityEventType eventType) override;

    // ETAndroidInputEvents
    bool ET_onInputEvent(AInputEvent* inputEvent) override;

private:

    bool hadleMotionEvent(AInputEvent* inputEvent);
    bool handleKeyEvent(AInputEvent* inputEvent);
};

#endif /* __ANDROIND_EVENT_MANGER_HPP__ */