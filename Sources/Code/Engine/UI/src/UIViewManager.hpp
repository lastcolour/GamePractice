#ifndef __UI_VIEW_MANAGER_HPP__
#define __UI_VIEW_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIView.hpp"
#include "LoadingScreenManager.hpp"

#include <vector>
#include <unordered_map>

class UIViewManager : public SystemLogic,
    public ETNode<ETUIViewManager> {
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
    void ET_onViewLoaded(UIViewType viewType, EntityId viewId) override;
    void ET_onViewAppeared(EntityId viewId) override;
    void ET_onViewDisappeared(EntityId viewId) override;

private:

    struct UIViewNode {
        UIViewType type;
        EntityId id;
    };

private:

    LoadingScreenManager loadingScreenMgr;
    std::vector<UIViewNode> stack;
    std::vector<EntityId> closingViews;
    bool isLoadingView;
};

#endif /* __UI_VIEW_MANAGER_HPP__ */