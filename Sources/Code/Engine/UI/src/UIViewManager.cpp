#include "UIViewManager.hpp"
#include "Core/ETLogger.hpp"
#include "Entity/ETEntityManger.hpp"
#include "UI/ETUIBox.hpp"
#include "Core/ETApplication.hpp"
#include "UIConfig.hpp"

#include <cassert>
#include <algorithm>

namespace {

const int Z_INDEX_VIEW_OFFSET = 1000;

} // namespace

UIViewManager::UIViewManager() :
    isLoadingView(false) {
}

UIViewManager::~UIViewManager() {
}

bool UIViewManager::init() {
    ETNode<ETUIViewManager>::connect(getEntityId());
    return true;
}

void UIViewManager::deinit() {
}

EntityId UIViewManager::getLoadedViewId(UIViewType viewType) const {
    auto it = loadedViews.find(viewType);
    if(it == loadedViews.end()) {
        return InvalidEntityId;
    }
    auto viewId = it->second;
    return viewId;
}

const char* UIViewManager::getViewName(UIViewType viewType) const {
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
        case UIViewType::None: {
            [[fallthrough]];
        }
        default: {
            assert(false && "Invalid view type");
        }
    }
    return viewName;
}

bool UIViewManager::ET_openView(UIViewType viewType) {
    if(isLoadingView) {
        LogError("[UIViewManager::ET_openView] Already loading another view");
    }
    EntityId viewId = getLoadedViewId(viewType);
    if(viewId.isValid()) {
        ET_onViewLoaded(viewType, viewId);
    } else {
        auto viewName = getViewName(viewType);
        ET_SendEvent(&ETAsyncEntityManager::ET_createAsyncEntity, viewName, [viewType](EntityId viewId){
            ET_QueueEvent(&ETUIViewManager::ET_onViewLoaded, viewType, viewId);
        });
    }
    return true;
}

void UIViewManager::ET_onViewLoaded(UIViewType viewType, EntityId viewId) {
    isLoadingView = false;
    if(viewId == InvalidEntityId) {
        LogWarning("[UIViewManager::ET_onViewLoaded] Can't create view!");
        return;
    }
    
    loadedViews[viewType] = viewId;

    int zIndex = 0;
    if(!stack.empty()) {
        auto topViewId = stack.back().id;
        ET_SendEventReturn(zIndex, topViewId, &ETUIElement::ET_getZIndex);
        zIndex += Z_INDEX_VIEW_OFFSET;
    }
    UIViewNode node;
    node.type = viewType;
    node.id = viewId;
    stack.push_back(node);

    ET_SendEvent(node.id, &ETUIElement::ET_setZIndex, zIndex);
    ET_SendEvent(&ETUIViewTransitionManager::ET_addAppearing, node.id);
}

void UIViewManager::ET_closeView(UIViewType viewType) {
    auto it = std::find_if(stack.begin(), stack.end(), [viewType](const UIViewNode& node){
        return viewType == node.type;
    });
    if(it == stack.end()) {
        LogWarning("[UIViewManager::ET_closeView] Can't find view to close");
        return;
    }
    ET_SendEvent(&ETUIViewTransitionManager::ET_addDisappearing, it->id);
    stack.erase(it);
}

UIViewType UIViewManager::ET_getActiveViewType() const {
    if(stack.empty()) {
        return UIViewType::None;
    }
    return stack.back().type;
}

EntityId UIViewManager::ET_getActiveViewId() const {
    if(stack.empty()) {
        return InvalidEntityId;
    }
    return stack.back().id;
}