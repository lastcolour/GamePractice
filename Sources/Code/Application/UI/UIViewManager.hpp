#ifndef __UI_VIEW_MANAGER_HPP__
#define __UI_VIEW_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "UI/UIETInterfaces.hpp"

class UIViewManager : public SystemLogic,
    public ETNode<ETUIViewManager> {
public:

    UIViewManager();
    virtual ~UIViewManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETUIViewManager
    bool ET_openView(const char* viewName) override;

private:

    EntityId activeViewId;
};

#endif /* __UI_VIEW_MANAGER_HPP__ */