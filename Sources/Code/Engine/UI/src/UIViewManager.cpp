#include "UIViewManager.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>

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

void UIViewManager::ET_openView(EntityId viewId) {
    assert(viewId.isValid());
    viewStack.push_back(viewId);
}

void UIViewManager::ET_closeView(EntityId viewId) {
    auto it = viewStack.begin();
    for(auto end = viewStack.end(); it != end; ++it) {
        if(*it == viewId) {
            break;
        }
    }
    if(it == viewStack.end()) {
        LogError("[UIViewManager::ET_closeView] Can't find view to close");
        return;
    }
    viewStack.erase(it);
}