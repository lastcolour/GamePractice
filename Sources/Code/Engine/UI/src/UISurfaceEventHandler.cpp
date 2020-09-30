#include "UISurfaceEventHandler.hpp"
#include "UI/ETUIView.hpp"
#include "UI/ETUIButton.hpp"
#include "UI/ETUIBox.hpp"
#include "UIUtils.hpp"

#include <algorithm>
#include <cassert>

UISurfaceEventHandler::UISurfaceEventHandler() {
}

UISurfaceEventHandler::~UISurfaceEventHandler() {
}

bool UISurfaceEventHandler::init() {
    ETNode<ETInputEvents>::connect(getEntityId());
    ETNode<ETSurfaceEvents>::connect(getEntityId());
    return true;
}

void UISurfaceEventHandler::deinit() {
    ETNode<ETInputEvents>::disconnect();
    ETNode<ETSurfaceEvents>::disconnect();
}

bool UISurfaceEventHandler::isHover(const Vec2i& pt, EntityId entId) const {
    AABB2Di elemBox(0);
    ET_SendEventReturn(elemBox, entId, &ETUIInteractionBox::ET_getHitBox);
    return UI::IsInsideBox(pt, elemBox);
}

std::vector<EntityId> UISurfaceEventHandler::getHoveredEntities(const Vec2i& pt) const {
    std::vector<EntityId> res;
    auto interactiveElems = ET_GetAll<ETUIInteractionBox>();
    for(auto elemId : interactiveElems) {
        if(isHover(pt, elemId)) {
            res.push_back(elemId);
        }
    }
    return res;
}

void UISurfaceEventHandler::onPress(const Vec2i& pt) {
    eventHandlers.clear();
    auto uiElems = getHoveredEntities(pt);
    for(auto elemId : uiElems) {
        EInputEventResult handleRes = EInputEventResult::Ignore;
        ET_SendEventReturn(handleRes, elemId, &ETUIInteractionBox::ET_onInputEvent, EActionType::Press, pt);
        if(handleRes == EInputEventResult::Accept) {
            eventHandlers.push_back(elemId);
        }
    }
}

void UISurfaceEventHandler::onMove(const Vec2i& pt) {
    for(auto& elemId : eventHandlers) {
        if(!elemId.isValid()) {
            continue;
        }
        EInputEventResult handleRes = EInputEventResult::Ignore;
        ET_SendEventReturn(handleRes, elemId, &ETUIInteractionBox::ET_onInputEvent, EActionType::Move, pt);
        if(handleRes == EInputEventResult::Ignore) {
            elemId = InvalidEntityId;
        }
    }
}

void UISurfaceEventHandler::onRelease(const Vec2i& pt) {
    for(auto& elemId : eventHandlers) {
        if(!elemId.isValid()) {
            continue;
        }
        ET_SendEvent(elemId, &ETUIInteractionBox::ET_onInputEvent, EActionType::Release, pt);
    }
    eventHandlers.clear();
}

void UISurfaceEventHandler::ET_onTouch(EActionType actionType, const Vec2i& pt) {
    switch(actionType)
    {
    case EActionType::Press:
        onPress(pt);
        break;
    case EActionType::Move:
        onMove(pt);
        break;
    case EActionType::Release:
        onRelease(pt);
        break;
    default:
        assert(false && "Invalid event type");
        break;
    }
}

void UISurfaceEventHandler::ET_onButton(EActionType actionType, EButtonId buttonId) {
    if(actionType == EActionType::Press && buttonId == EButtonId::Back) {
        ET_SendEvent(&ETUIEventManager::ET_onEvent, UIEventType::OnBackButton);
    }
}

void UISurfaceEventHandler::ET_onSurfaceHidden() {
    ET_SendEvent(&ETUIEventManager::ET_onEvent, UIEventType::OnSurfaceHidden);
}

void UISurfaceEventHandler::ET_onSurfaceShown() {
    ET_SendEvent(&ETUIEventManager::ET_onEvent, UIEventType::OnSurfaceShown);
}
