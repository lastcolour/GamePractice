#ifndef __ANDROIND_EVENT_MANGER_HPP__
#define __ANDROIND_EVENT_MANGER_HPP__

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Platforms/Android/ETAndroidInterfaces.hpp"

class AndroidEventManager : public SystemLogic,
    public ETNode<ETTimerEvents>,
    public ETNode<ETAndroidActivityEvents>,
    public ETNode<ETAndroidInputEvents> {
public:

    AndroidEventManager();
    virtual ~AndroidEventManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETTimerEvents
    void ET_onTick(float dt) override;

    // ETAndroidActivityEvents
    void ET_onActivityEvent(ActivityEventType eventType) override;

    // ETAndroidInputEvents
    bool ET_onInputEvent(AInputEvent* inputEvent) override;

private:

    bool hadleMotionEvent(AInputEvent* inputEvent);
    bool handleKeyEvent(AInputEvent* inputEvent);
};

#endif /* __ANDROIND_EVENT_MANGER_HPP__ */