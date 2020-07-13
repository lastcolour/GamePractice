#ifndef __UI_VIEW_MANAGER_HPP__
#define __UI_VIEW_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIView.hpp"

#include <vector>

class UIViewManager : public SystemLogic,
    public ETNode<ETUIViewManager>,
    public ETNode<ETUIViewAppearAnimationEvents> {
public:

    UIViewManager();
    virtual ~UIViewManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETUIViewManager
    bool ET_openView(UIViewType viewType) override;
    void ET_closeView(UIViewType viewType) override;
    UIViewType ET_getActiveViewType() const override;
    EntityId ET_getActiveViewId() const override;

    // ETUIViewAppearAnimationEvents
    void ET_onViewAppeared() override;
    void ET_onViewDisappeared() override;

private:

    struct UIViewNode {
        UIViewType type;
        EntityId id;
    };

private:

    std::vector<UIViewNode> stack;
};

#endif /* __UI_VIEW_MANAGER_HPP__ */