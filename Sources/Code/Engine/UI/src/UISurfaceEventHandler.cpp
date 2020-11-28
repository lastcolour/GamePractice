#include "UISurfaceEventHandler.hpp"
#include "UI/ETUIButton.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/ETUIViewScript.hpp"
#include "UIUtils.hpp"

#include <algorithm>
#include <cassert>

namespace {

bool isHoverHitBox(const Vec2i& pt, EntityId entId) {
    AABB2Di elemBox(0);
    ET_SendEventReturn(elemBox, entId, &ETUIInteractionBox::ET_getHitBox);
    return elemBox.isInside(pt);
}

bool canAcceptEvent(EntityId entId) {
    bool result = false;
    ET_SendEventReturn(result, entId, &ETUIElement::ET_isHidden);
    if(result) {
        return false;
    }
    result = false;
    ET_SendEventReturn(result, entId, &ETUIElement::ET_isEnabled);
    if(!result) {
        return false;
    }
    if(!UI::IsRootViewHasFocus(entId)) {
        return false;
    }
    return true;
}

} // namespace

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

std::vector<EntityId> UISurfaceEventHandler::getHoveredEntities(const Vec2i& pt) const {
    std::vector<EntityId> res;
    auto interactiveElems = ET_GetAll<ETUIInteractionBox>();
    for(auto elemId : interactiveElems) {
        if(!canAcceptEvent(elemId)) {
            continue;
        }
        if(isHoverHitBox(pt, elemId)) {
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
        if(canAcceptEvent(elemId)) {
            ET_SendEventReturn(handleRes, elemId, &ETUIInteractionBox::ET_onInputEvent, EActionType::Move, pt);
        }
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
    EntityId viewId;
    ET_SendEventReturn(viewId, &ETUIViewManager::ET_getActiveViewId);
    if(!viewId.isValid()) {
        return;
    }
    if(actionType == EActionType::Press && buttonId == EButtonId::Back) {
        UIEvent backEvent{InvalidEntityId, UIEvent::EventType::OnBackButton};
        ET_SendEvent(viewId, &ETUIViewScript::ET_onEvent, backEvent);
    }
}

void UISurfaceEventHandler::ET_onSurfaceHidden() {
}

void UISurfaceEventHandler::ET_onSurfaceShown() {
}