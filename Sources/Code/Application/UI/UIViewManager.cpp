#include "UI/UIViewManager.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"

UIViewManager::UIViewManager() {
}

UIViewManager::~UIViewManager() {
}

bool UIViewManager::init() {
    ETNode<ETUIViewManager>::connect(getEntityId());
    return true;
}

void UIViewManager::deinit() {
    ETNode<ETUIViewManager>::disconnect();
}

bool UIViewManager::ET_openView(const char* viewName) {
    if(!viewName || !viewName[0]) {
        LogWarning("[UIViewManager::ET_openView] Can't open empty view");
        return false;
    }
    EntityId newViewId;
    ET_SendEventReturn(newViewId, &ETGameObjectManager::ET_createGameObject, viewName);
    if(!newViewId.isValid()) {
        LogWarning("[UIViewManager::ET_openView] Can't open view: %s", viewName);
        return false;
    }

    ET_SendEvent(&ETGameObjectManager::ET_destroyObject, activeViewId);
    activeViewId = newViewId;
    return true;
}