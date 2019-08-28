#include "UI/UIViewStack.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Entity/ETEntityInterfaces.hpp"

UIViewStack::UIViewStack() :
    isPopping(false) {
}

UIViewStack::~UIViewStack() {
}

bool UIViewStack::init() {
    ETNode<ETUIViewStack>::connect(getEntityId());
    ETNode<ETUIViewSwitcherEvents>::connect(getEntityId());
    return true;
}

void UIViewStack::deinit() {
    ETNode<ETUIViewStack>::disconnect();
    ETNode<ETUIViewSwitcherEvents>::disconnect();
}

EntityId UIViewStack::ET_pushView(const char* viewName) {
    if(!viewName || !viewName[0]) {
        LogWarning("[UIViewStack::ET_pushView] Can't open empty view");
        return InvalidEntityId;
    }
    EntityId newViewId;
    ET_SendEventReturn(newViewId, &ETEntityManager::ET_createEntity, viewName);
    if(!newViewId.isValid()) {
        LogWarning("[UIViewStack::ET_pushView] Can't open view: %s", viewName);
        return InvalidEntityId;
    }
    EntityId activeViewId = ET_getActiveViewId();
    if(activeViewId.isValid()) {
        ET_SendEvent(&ETUIViewSwitcher::ET_swtichView, newViewId, activeViewId);
    }
    viewStack.push_back(newViewId);
    return newViewId;
}

EntityId UIViewStack::ET_getActiveViewId() const {
    if(!viewStack.empty()) {
        return viewStack.back();
    }
    return InvalidEntityId;
}

void UIViewStack::ET_popView() {
    if(viewStack.size() < 2) {
        LogError("[UIViewStack::ET_popView] Can't perform view on stack of size: %d", viewStack.size());
        return;
    }
    EntityId topViewId = viewStack.back();
    viewStack.pop_back();
    isPopping = true;
    ET_SendEvent(&ETUIViewSwitcher::ET_swtichView, viewStack.back(), topViewId);
}

void UIViewStack::ET_clearAllAndPushNewView(const char* viewName) {
    for(auto it = viewStack.rbegin(), end = viewStack.rend(); it != end; ++it) {
        EntityId viewId = *it;
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, viewId);
    }
    viewStack.clear();
    ET_pushView(viewName);
}

void UIViewStack::ET_onViewSwitchedOut(EntityId viewId) {
    if(isPopping) {
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, viewId);
        isPopping = false;
    }
}