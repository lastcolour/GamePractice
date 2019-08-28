#include "UI/UISurfaceEventHandler.hpp"
#include "UI/ETUIInterfaces.hpp"

namespace {

const char* BACK_BUTTON_EVENT_NAME = "OnBackButton";

} // namespace

UISurfaceEventHandler::UISurfaceEventHandler() {
}

UISurfaceEventHandler::~UISurfaceEventHandler() {
}

bool UISurfaceEventHandler::init() {
    ETNode<ETInputEvents>::connect(getEntityId());
    return true;
}

void UISurfaceEventHandler::deinit() {
    ETNode<ETInputEvents>::disconnect();
}

bool UISurfaceEventHandler::isHover(const Vec2i& pt, EntityId entId) const {
    AABB2Di elemBox(0);
    ET_SendEventReturn(elemBox, entId, &ETUIInteractionBox::ET_getHitBox);
    return pt >= elemBox.bot && pt <= elemBox.top;
}

EntityId UISurfaceEventHandler::getHoveredEntity(const Vec2i& pt) const {
    auto interactiveElems = ET_GetAll<ETUIInteractionBox>();
    for(auto elemId : interactiveElems) {
        if (isHover(pt, elemId)) {
            return elemId;
        }
    }
    return InvalidEntityId;
}

void UISurfaceEventHandler::onPress(const Vec2i& pt) {
    hoveredElemId = getHoveredEntity(pt);
    if(hoveredElemId.isValid()) {
        pressElemId = hoveredElemId;
        ET_SendEvent(hoveredElemId, &ETUIInteractionBox::ET_onHover, true);
    }
}

void UISurfaceEventHandler::onMove(const Vec2i& pt) {
    auto currHoveredId = getHoveredEntity(pt);
    if (currHoveredId == InvalidEntityId) {
        if(hoveredElemId.isValid()) {
            ET_SendEvent(hoveredElemId, &ETUIInteractionBox::ET_onHover, false);
            hoveredElemId = InvalidEntityId;
        }
    } else if (currHoveredId != hoveredElemId) {
        if(hoveredElemId.isValid()) {
            ET_SendEvent(hoveredElemId, &ETUIInteractionBox::ET_onHover, false);
        }
        ET_SendEvent(currHoveredId, &ETUIInteractionBox::ET_onHover, true);
        hoveredElemId = currHoveredId;
    }
}

void UISurfaceEventHandler::onRelease(const Vec2i& pt) {
    auto releseElemId = getHoveredEntity(pt);
    if (hoveredElemId.isValid()) {
        ET_SendEvent(hoveredElemId, &ETUIInteractionBox::ET_onHover, false);
        if(releseElemId == pressElemId) {
            ET_SendEvent(hoveredElemId, &ETUIInteractionBox::ET_onPress);
        }
    }
    hoveredElemId = InvalidEntityId;
    pressElemId = InvalidEntityId;
}

void UISurfaceEventHandler::ET_onTouch(EActionType actionType, const Vec2i& pt) {
    switch (actionType)
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
        break;
    }
}

void UISurfaceEventHandler::ET_onButton(EActionType actionType, EButtonId buttonId) {
    if(actionType == EActionType::Press && buttonId == EButtonId::Back) {
        ET_SendEvent(&ETUIEventManager::ET_onEvent, BACK_BUTTON_EVENT_NAME);
    }
}