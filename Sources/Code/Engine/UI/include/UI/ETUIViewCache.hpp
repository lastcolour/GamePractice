#ifndef __ET_UI_VIEW_CACHE_HPP__
#define __ET_UI_VIEW_CACHE_HPP__

#include "UI/ETUIView.hpp"

class EntityLoadResult;

struct ETUIViewCache {
    virtual ~ETUIViewCache() = default;
    virtual EntityId ET_getViewId(UIViewType viewType) const = 0;
    virtual EntityId ET_loadView(UIViewType viewType) = 0;
    virtual void ET_asyncLoadView(UIViewType viewType) = 0;
    virtual void ET_onAsyncLoadDone(UIViewType viewType, std::shared_ptr<EntityLoadResult> result) = 0;
};

struct ETUIViewCacheEvents {
    virtual ~ETUIViewCacheEvents() = default;
    virtual void ET_onViewLoaded(UIViewType viewType, std::shared_ptr<EntityLoadResult> result) = 0;
};

#endif /* __ET_UI_VIEW_CACHE_HPP__ */