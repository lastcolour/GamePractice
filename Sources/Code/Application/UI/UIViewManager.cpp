#include "UI/UIViewManager.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"

UIViewManager::UIViewManager() {
}

UIViewManager::~UIViewManager() {
}

bool UIViewManager::init() {
    ETNode<ETUIViewManager>::connect(getEntityId());
    ETNode<ETUIViewSwitcherEvents>::connect(getEntityId());
    return true;
}

void UIViewManager::deinit() {
    ETNode<ETUIViewManager>::disconnect();
    ETNode<ETUIViewSwitcherEvents>::disconnect();
}

EntityId UIViewManager::ET_openView(const char* viewName) {
    if(!viewName || !viewName[0]) {
        LogWarning("[UIViewManager::ET_openView] Can't open empty view");
        return InvalidEntityId;
    }
    EntityId newViewId;
    ET_SendEventReturn(newViewId, &ETGameObjectManager::ET_createGameObject, viewName);
    if(!newViewId.isValid()) {
        LogWarning("[UIViewManager::ET_openView] Can't open view: %s", viewName);
        return InvalidEntityId;
    }

    if (activeViewId.isValid()) {
        ET_SendEvent(&ETUIViewSwitcher::ET_swtichView, newViewId, activeViewId);
    }
    activeViewId = newViewId;
    return activeViewId;
}

void UIViewManager::ET_closeView(EntityId viewId) {
    if(activeViewId == viewId && activeViewId.isValid()) {
        ET_SendEvent(&ETGameObjectManager::ET_destroyObject, activeViewId);
        activeViewId = InvalidEntityId;
    }
}

void UIViewManager::ET_onViewSwitchedOut(EntityId viewId) {
    ET_SendEvent(&ETGameObjectManager::ET_destroyObject, viewId);
}