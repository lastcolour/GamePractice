#include "UIEventManager.hpp"

UIEventManager::UIEventManager() {
}

UIEventManager::~UIEventManager() {
}

bool UIEventManager::init() {
    ETNode<ETUIButtonEventManager>::connect(getEntityId());
    ETNode<ETUIEventManager>::connect(getEntityId());
    return true;
}

void UIEventManager::deinit() {
}

EntityId UIEventManager::ET_getActiveButton() const {
    return activeButtonId;
}

void UIEventManager::ET_setActiveButton(EntityId buttonId) {
    activeButtonId = buttonId;
}

void UIEventManager::ET_onEvent(UIEventType eventType) {
}