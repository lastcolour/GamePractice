#include "UI/UIEventManager.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"

enum class EUIEventType {
    OnMainStart,
};

UIEventManager::UIEventManager() {
    eventMap["OnMainStart"] = EUIEventType::OnMainStart;
}

UIEventManager::~UIEventManager() {
}

bool UIEventManager::init() {
    ETNode<ETUIEventManager>::connect(getEntityId());
    return true;
}

void UIEventManager::deinit() {
}

void UIEventManager::processEvent(EUIEventType eventType) {
    switch (eventType)
    {
        case EUIEventType::OnMainStart:
        {
            ET_SendEvent(&ETUIViewManager::ET_openView, "UI/GameView/Root.json");
            break;
        }
        default:
            break;
    }
}

void UIEventManager::ET_onEvent(const std::string& eventName) {
    auto it = eventMap.find(eventName);
    if (it == eventMap.end()) {
        LogWarning("[UIEventManager::ET_onEvent] Unknown event name: %s", eventName);
        return;
    }
    processEvent(it->second);
}