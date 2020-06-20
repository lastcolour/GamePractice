#ifndef __UI_VIEW_STACK_HPP__
#define __UI_VIEW_STACK_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIView.hpp"

class UIViewStack : public SystemLogic,
    public ETNode<ETUIViewStack>,
    public ETNode<ETUIViewSwitcherEvents> {
public:

    UIViewStack();
    virtual ~UIViewStack();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETUIViewStack
    void ET_pushView(const char* viewName) override;
    void ET_forcePushView(const char* viewName) override;
    void ET_clearAllAndPushNewView(const char* viewName) override;
    void ET_popView() override;
    void ET_forceReset() override;
    EntityId ET_getActiveViewId() const override;

    // ETUIViewSwitcherEvents
    void ET_onViewSwitchFinished(EntityId viewId) override;

private:

    enum class ETaskState {
        None = 0,
        WaitPush,
        Pushing,
        WaitPop,
        Popping
    };

    struct StackTask {
        std::string viewName;
        EntityId viewId;
        ETaskState state;
    };

private:

    bool initPush(const std::string& viewName);
    void startNextTask();
    void notifyNormalPush();
    void notifyNormalPop();

private:

    std::vector<StackTask> taskQueue;
    std::vector<EntityId> viewStack;
};

#endif /* __UI_VIEW_STACK_HPP__ */