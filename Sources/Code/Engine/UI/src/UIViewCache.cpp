#include "UIViewCache.hpp"
#include "UIConfig.hpp"
#include "Core/ETApplication.hpp"
#include "Entity/ETEntityManger.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>

namespace {

const char* getViewName(UIViewType viewType) {
    const char* viewName = nullptr;
    auto uiConfig = ET_getShared<UIConfig>();
    switch(viewType) {
        case UIViewType::Main: {
            viewName = uiConfig->mainView.c_str();
            break;
        }
        case UIViewType::Game: {
            viewName = uiConfig->gameView.c_str();
            break;
        }
        case UIViewType::Background: {
            viewName = uiConfig->backgroundView.c_str();
            break;
        }
        case UIViewType::EndGame: {
            viewName = uiConfig->endGameView.c_str();
            break;
        }
        case UIViewType::Loading: {
            viewName = uiConfig->loadingView.c_str();
            break;
        }
        case UIViewType::PauseGame: {
            viewName = uiConfig->pauseView.c_str();
            break;
        }
        case UIViewType::None: {
            [[fallthrough]];
        }
        default: {
            assert(false && "Invalid view type");
        }
    }
    return viewName;
}

} // namespace

UIViewCache::UIViewCache() {
}

UIViewCache::~UIViewCache() {
}

bool UIViewCache::init() {
    ETNode<ETUIViewCache>::connect(getEntityId());
    return true;
}

void UIViewCache::deinit() {
}

EntityId UIViewCache::ET_getViewId(UIViewType viewType) const {
    auto it = loadedViews.find(viewType);
    if(it == loadedViews.end()) {
        return InvalidEntityId;
    }
    auto viewId = it->second;
    return viewId;
}

void UIViewCache::ET_onAsyncLoadDone(UIViewType viewType, EntityId viewId) {
    assert(viewType != UIViewType::None && "Invalid view type");
    if(!viewId.isValid()) {
        LogError("[UIViewCache::ET_onAsyncLoadDone] Async load of view '%s' failed", getViewName(viewType));
        return;
    }
    loadedViews[viewType] = viewId;
    ET_SendEvent(&ETUIViewManager::ET_onViewLoaded, viewType, viewId);
}

void UIViewCache::ET_asyncLoadView(UIViewType viewType) {
    auto viewName = getViewName(viewType);
    ET_SendEvent(&ETAsyncEntityManager::ET_createAsyncEntity, viewName, [viewType](EntityId viewId){
        ET_QueueEvent(&ETUIViewCache::ET_onAsyncLoadDone, viewType, viewId);
    });
}

EntityId UIViewCache::ET_loadView(UIViewType viewType) {
    auto viewId = ET_getViewId(viewType);
    if(viewId.isValid()) {
        return viewId;
    }
    auto viewName = getViewName(viewType);
    ET_SendEventReturn(viewId, &ETEntityManager::ET_createEntity, viewName);
    if(!viewId.isValid()) {
        LogError("[UIViewCache::ET_loadView] Can't load view: '%s'", viewName);
    }
    return viewId;
}
