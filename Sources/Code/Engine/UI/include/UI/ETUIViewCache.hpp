#ifndef __ET_UI_VIEW_CACHE_HPP__
#define __ET_UI_VIEW_CACHE_HPP__

#include "Core/Core.hpp"
#include "UI/ETUIView.hpp"

struct ETUIViewCache {
    virtual ~ETUIViewCache() = default;
    virtual EntityId ET_getViewId(UIViewType viewType) const = 0;
    virtual EntityId ET_loadView(UIViewType viewType) = 0;
    virtual void ET_asyncLoadView(UIViewType viewType) = 0;
    virtual void ET_onAsyncLoadDone(UIViewType viewType, EntityId viewId) = 0;
};

struct ETUIViewCacheEvents {
    virtual ~ETUIViewCacheEvents() = default;
    virtual void ET_onViewLoaded(UIViewType viewType, EntityId viewId) = 0;
};

#endif /* __ET_UI_VIEW_CACHE_HPP__ */