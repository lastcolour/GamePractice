#ifndef __UI_VIEW_MANAGER_HPP__
#define __UI_VIEW_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIView.hpp"

class UIViewManager : public SystemLogic,
    public ETNode<ETUIViewManager> {
public:

    UIViewManager();
    virtual ~UIViewManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETUIViewManager
    void ET_openView(EntityId viewId) override;
    void ET_closeView(EntityId viewId) override;

private:

    std::vector<EntityId> viewStack;
};

#endif /* __UI_VIEW_MANAGER_HPP__ */