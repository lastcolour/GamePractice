#include "UIViewCache.hpp"
#include "Config/UIConfig.hpp"
#include "Core/ETApplication.hpp"
#include "Entity/ETEntityManager.hpp"
#include "UI/ETUIViewPort.hpp"
#include "Entity/EntityLoadResult.hpp"
#include "UI/ETUIBox.hpp"
#include "Core/GlobalData.hpp"

#include <cassert>

namespace {

const char* getViewName(UIViewType viewType) {
    const char* viewName = nullptr;
    const auto& viewsConfig = GetGlobal<UIConfig>()->views;
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

void UIViewCache::ET_onAsyncLoadDone(UIViewType viewType, std::shared_ptr<EntityLoadResult> result) {
    assert(viewType != UIViewType::None && "Invalid view type");
    if(!result) {
        assert(false && "Invalid async load result");
        return;
    }
    auto viewId = result->finishLoad();
    if(!viewId.isValid()) {
        LogError("[UIViewCache::ET_onAsyncLoadDone] Async load of view '%s' failed", getViewName(viewType));
        return;
    }

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    ET_SendEvent(viewId, &ETUIViewPortEvents::ET_onViewPortChanged, viewPort);

    ET_SendEvent(viewId, &ETUIElement::ET_hide);

    loadedViews[viewType] = viewId;
    ET_SendEvent(&ETUIViewManager::ET_onViewLoaded, viewType, viewId);
}

void UIViewCache::ET_asyncLoadView(UIViewType viewType) {
    auto viewName = getViewName(viewType);
    ET_SendEvent(&ETAsyncEntityManager::ET_createAsyncEntity, viewName, [viewType](std::shared_ptr<EntityLoadResult> result){
        ET_QueueEvent(&ETUIViewCache::ET_onAsyncLoadDone, viewType, result);
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
