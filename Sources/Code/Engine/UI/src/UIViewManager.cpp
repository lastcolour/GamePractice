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

UIViewManager::UIViewManager() {
}

UIViewManager::~UIViewManager() {
}

bool UIViewManager::init() {
    ETNode<ETUIViewManager>::connect(getEntityId());
    return true;
}

void UIViewManager::deinit() {
}

EntityId UIViewManager::getViewId(UIViewType viewType) {
    auto it = loadedViews.find(viewType);
    if(it != loadedViews.end()) {
        return it->second;
    }
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
            return InvalidEntityId;
        }
    }
    EntityId viewId;
    ET_SendEventReturn(viewId, &ETEntityManager::ET_createEntity, viewName);
    if(!viewId.isValid()) {
        LogError("[ UIViewManager::getViewId] Can't create view: '%s'", viewName);
        return InvalidEntityId;
    }
    loadedViews[viewType] = viewId;
    ET_SendEvent(viewId, &ETUIElement::ET_hide);
    return viewId;
}

bool UIViewManager::ET_openView(UIViewType viewType) {
    int zIndex = 0;
    if(!stack.empty()) {
        auto topViewId = stack.back().id;
        ET_SendEventReturn(zIndex, topViewId, &ETUIElement::ET_getZIndex);
        zIndex += Z_INDEX_VIEW_OFFSET;
    }
    UIViewNode node;
    node.type = viewType;
    node.id = getViewId(viewType);
    if(node.id == InvalidEntityId) {
        LogWarning("[UIViewManager::ET_openView] Can't create view!");
        return false;
    }
    stack.push_back(node);
    ET_SendEvent(node.id, &ETUIElement::ET_setZIndex, zIndex);
    ET_SendEvent(&ETUIViewTransitionManager::ET_addAppearing, node.id);
    return true;
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