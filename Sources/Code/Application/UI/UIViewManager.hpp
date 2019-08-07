#ifndef __UI_VIEW_MANAGER_HPP__
#define __UI_VIEW_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "UI/UIETInterfaces.hpp"

class UIViewManager : public SystemLogic,
    public ETNode<ETUIViewManager>,
    public ETNode<ETUIViewSwitcherEvents> {
public:

    UIViewManager();
    virtual ~UIViewManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETUIViewManager
    EntityId ET_openView(const char* viewName) override;
    void ET_closeView(EntityId viewId) override;

    // ETUIViewSwitcherEvents
    void ET_onViewSwitchedOut(EntityId viewId) override;

private:

    EntityId activeViewId;
};

#endif /* __UI_VIEW_MANAGER_HPP__ */