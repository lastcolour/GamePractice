#ifndef __UI_VIEW_CACHE_HPP__
#define __UI_VIEW_CACHE_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIViewCache.hpp"

class UIViewCache : public SystemLogic,
    public ETNode<ETUIViewCache> {
public:

    UIViewCache();
    virtual ~UIViewCache();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETUIViewCache
    EntityId ET_getViewId(UIViewType viewType) const override;
    EntityId ET_loadView(UIViewType viewType) override;
    void ET_asyncLoadView(UIViewType viewType) override;
    void ET_onAsyncLoadDone(UIViewType viewType, std::shared_ptr<EntityLoadResult> result) override;

private:

    std::unordered_map<UIViewType, EntityId> loadedViews;
};

#endif /* __UI_VIEW_CACHE_HPP__ */