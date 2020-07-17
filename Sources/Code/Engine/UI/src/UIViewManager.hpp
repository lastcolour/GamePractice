#ifndef __UI_VIEW_MANAGER_HPP__
#define __UI_VIEW_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIView.hpp"

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

private:

    struct UIViewNode {
        UIViewType type;
        EntityId id;
    };

private:

    EntityId getViewId(UIViewType viewType);

private:

    std::unordered_map<UIViewType, EntityId> loadedViews;
    std::vector<UIViewNode> stack;
};

#endif /* __UI_VIEW_MANAGER_HPP__ */