#include "UISurfaceEventHandler.hpp"
#include "UI/ETUIButton.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/ETUIViewScript.hpp"
#include "UIUtils.hpp"

#include <cassert>

namespace {

bool isCulledByHostBoxes(EntityId entId, const Vec2i& pt, const AABB2D& checkBox) {
    EntityId currCheckEnt = entId;

    while(true) {
        EntityId hostId;
        ET_SendEventReturn(hostId, currCheckEnt, &ETUIElement::ET_getHostLayout);
        if(!hostId.isValid()) {
            break;
        }
        AABB2D hostBox(0.f);
        ET_SendEventReturn(hostBox, hostId, &ETUIElement::ET_getBox);
        if(!hostBox.isInside(static_cast<float>(pt.x), static_cast<float>(pt.y))) {
            return true;
        }
        currCheckEnt = hostId;
    }

    return false;
}

bool isHitBoxHovered(const Vec2i& pt, EntityId entId) {
    AABB2D elemBox(0.f);
    ET_SendEventReturn(elemBox, entId, &ETUIElement::ET_getBox);
    if(!elemBox.isInside(static_cast<float>(pt.x), static_cast<float>(pt.y))) {
        return false;
    }
    if(isCulledByHostBoxes(entId, pt, elemBox)) {
        return false;
    }
    return true;
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

UISurfaceEventHandler::UISurfaceEventHandler() :
    lastTouchPt(0) {
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
        if(isHitBoxHovered(pt, elemId)) {
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

void UISurfaceEventHandler::onRelease(const Vec2i& pt, bool releaseIgnore) {
    EActionType releaseType = releaseIgnore ? EActionType::ReleaseAndIgnore : EActionType::Release;
    for(auto& elemId : eventHandlers) {
        if(!elemId.isValid()) {
            continue;
        }
        ET_SendEvent(elemId, &ETUIInteractionBox::ET_onInputEvent, releaseType, pt);
    }
    eventHandlers.clear();
}

void UISurfaceEventHandler::ET_onTouch(const TouchEvent& event) {
    lastTouchPt = event.pt;
    switch(event.actionType)
    {
    case EActionType::Press:
        onPress(event.pt);
        break;
    case EActionType::Move:
        onMove(event.pt);
        break;
    case EActionType::Release: {
        bool releaseIgnore = false;
        onRelease(event.pt, releaseIgnore);
        break;
    }
    default:
        assert(false && "Invalid event type");
        break;
    }
}

void UISurfaceEventHandler::ET_onButton(const ButtonEvent& event) {
    EntityId viewId;
    ET_SendEventReturn(viewId, &ETUIViewManager::ET_getActiveViewId);
    if(!viewId.isValid()) {
        return;
    }
    if(event.actionType == EActionType::Press && event.buttonId == EButtonId::Back) {
        UIEvent backEvent{InvalidEntityId, UIEvent::EventType::OnBackButton};
        ET_SendEvent(viewId, &ETUIViewScript::ET_onEvent, backEvent);
    }
}

void UISurfaceEventHandler::ET_onSurfaceLostFocus() {
    bool releaseIgnore = true;
    onRelease(lastTouchPt, releaseIgnore);
}