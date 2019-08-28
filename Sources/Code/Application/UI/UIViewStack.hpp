#ifndef __UI_VIEW_STACK_HPP__
#define __UI_VIEW_STACK_HPP__

#include "Core/SystemLogic.hpp"
#include "UI/ETUIInterfaces.hpp"

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
    EntityId ET_pushView(const char* viewName) override;
    void ET_clearAllAndPushNewView(const char* viewName) override;
    void ET_popView() override;
    EntityId ET_getActiveViewId() const override;

    // ETUIViewSwitcherEvents
    void ET_onViewSwitchedOut(EntityId viewId) override;

private:

    bool isPopping;
    std::vector<EntityId> viewStack;
};

#endif /* __UI_VIEW_STACK_HPP__ */