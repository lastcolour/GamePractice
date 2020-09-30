#include "UIViewCache.hpp"
#include "Config/UIConfig.hpp"
#include "Core/ETApplication.hpp"
#include "Entity/ETEntityManger.hpp"
#include "Core/ETLogger.hpp"
#include "UI/ETUIViewPort.hpp"

#include <cassert>

namespace {

const char* getViewName(UIViewType viewType) {
    const char* viewName = nullptr;
    const auto& viewsConfig = ET_getShared<UIConfig>()->views;
    switch(viewType) {
        case UIViewType::Main: {
            viewName = viewsConfig.mainView.c_str();
            break;
        }
        case UIViewType::Game: {
            viewName = viewsConfig.gameView.c_str();
            break;
        }
        case UIViewType::Background: {
            viewName = viewsConfig.backgroundView.c_str();
            break;
        }
        case UIViewType::EndGame: {
            viewName = viewsConfig.endGameView.c_str();
            break;
        }
        case UIViewType::Loading: {
            viewName = viewsConfig.loadingView.c_str();
            break;
        }
        case UIViewType::PauseGame: {
            viewName = viewsConfig.pauseView.c_str();
            break;
        }
        case UIViewType::Levels: {
            viewName = viewsConfig.levelsView.c_str();
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

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    ET_SendEvent(viewId, &ETUIViewPortEvents::ET_onViewPortChanged, viewPort);

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
