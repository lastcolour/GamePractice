#include "UIViewManager.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/ETUIViewCache.hpp"
#include "UI/ETLoadingScreen.hpp"
#include "UI/ETUIViewScript.hpp"
#include "UIUtils.hpp"

#include <cassert>

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

bool UIViewManager::ET_openView(UIViewType viewType) {
    if(isLoadingView) {
        LogError("[UIViewManager::ET_openView] Already loading another view");
    }
    for(auto& view : stack) {
        if(view.type == viewType) {
            LogError("[UIViewManager::ET_openView] Can't open view that already on the stack: '%s'",
                UI::GetViewTypeName(viewType));
            assert(false && "Invalid view open request");
            return false;
        }
    }
    if(!stack.empty()) {
        auto topViewId = stack.back().id;
        ET_SendEvent(topViewId, &ETUIView::ET_setFocus, false);
    }
    EntityId viewId;
    ET_SendEventReturn(viewId, &ETUIViewCache::ET_getViewId, viewType);
    if(viewId.isValid()) {
        ET_onViewLoaded(viewType, viewId);
    } else {
        isLoadingView = true;
        ET_SendEvent(&ETLoadingScreenManager::ET_showLoadingScreen);
        ET_SendEvent(&ETUIViewCache::ET_asyncLoadView, viewType);
    }
    return true;
}

void UIViewManager::ET_onViewLoaded(UIViewType viewType, EntityId viewId) {
    isLoadingView = false;
    bool loadingScreenOpened = false;
    ET_SendEventReturn(loadingScreenOpened, &ETLoadingScreenManager::ET_isLoadingScreenActive);
    if(loadingScreenOpened) {
        ET_SendEvent(&ETLoadingScreenManager::ET_hideLoadingScreen);
    }

    auto viewTypeStr = UI::GetViewTypeName(viewType);

    if(viewId == InvalidEntityId) {
        LogWarning("[UIViewManager::ET_onViewLoaded] Can't load '%s' view!", viewTypeStr);
        if(!stack.empty()) {
            auto topViewId = stack.back().id;
            ET_SendEvent(topViewId, &ETUIView::ET_setFocus, true);
        }
        return;
    }

    auto it = std::find_if(stack.begin(), stack.end(), [viewType](const UIViewNode& node){
        return viewType == node.type;
    });

    if(it != stack.end()) {
        LogError("[UIViewManager::ET_onViewLoaded] View '%s' already opened", viewTypeStr);
        return;
    }

    LogDebug("[UIViewManager::ET_onViewLoaded] Open view: '%s'", viewTypeStr);

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

void UIViewManager::ET_onViewStartAppearing(EntityId viewId) {
    ET_SendEvent(viewId, &ETUIViewScript::ET_onViewOpened);
}

void UIViewManager::ET_closeView(UIViewType viewType) {
    assert(viewType != UIViewType::None && "Invalid view type");
    auto it = std::find_if(stack.begin(), stack.end(), [viewType](const UIViewNode& node){
        return viewType == node.type;
    });
    auto viewTypeStr = UI::GetViewTypeName(viewType);
    if(it == stack.end()) {
        LogWarning("[UIViewManager::ET_closeView] Can't find view '%s' to close", viewTypeStr);
        return;
    }
    EntityId viewId = it->id;
    stack.erase(it);
    closingViews.push_back(viewId);
    LogDebug("[UIViewManager::ET_closeView] Close view: '%s'", viewTypeStr);
    ET_SendEvent(viewId, &ETUIView::ET_setFocus, false);
    ET_SendEvent(&ETUIViewTransitionManager::ET_addDisappearing, viewId);
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

void UIViewManager::ET_onViewAppeared(EntityId viewId) {
    ET_SendEvent(viewId, &ETUIView::ET_setFocus, true);
}

void UIViewManager::ET_onViewDisappeared(EntityId viewId) {
    ET_SendEvent(viewId, &ETUIViewScript::ET_onViewClosed);
    auto it = std::find(closingViews.begin(), closingViews.end(), viewId);
    if(it == closingViews.end()) {
        return;
    }
    closingViews.erase(it);
    if(stack.empty()) {
        return;
    }
    bool hasActiveTransition = false;
    ET_SendEventReturn(hasActiveTransition, &ETUIViewTransitionManager::ET_hasActiveTransition);
    if(!hasActiveTransition) {
        auto topViewId = stack.back().id;
        ET_SendEvent(topViewId, &ETUIView::ET_setFocus, true);
    }
}