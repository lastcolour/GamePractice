#include "UI/UISurfaceTouchManager.hpp"
#include "UI/UIETInterfaces.hpp"

UISurfaceTouchManager::UISurfaceTouchManager() {
}

UISurfaceTouchManager::~UISurfaceTouchManager() {
}

bool UISurfaceTouchManager::init() {
    ETNode<ETInputEvents>::connect(getEntityId());
    return true;
}

void UISurfaceTouchManager::deinit() {
    ETNode<ETInputEvents>::disconnect();
}

bool UISurfaceTouchManager::isHover(const Vec2i& pt, EntityId entId) const {
    AABB2Di elemBox(0);
    ET_SendEventReturn(elemBox, entId, &ETUIInteractionBox::ET_getHitBox);
    return pt >= elemBox.bot && pt <= elemBox.top;
}

EntityId UISurfaceTouchManager::getHoveredEntity(const Vec2i& pt) const {
    auto interactiveElems = ET_GetAll<ETUIInteractionBox>();
    for(auto elemId : interactiveElems) {
        if (isHover(pt, elemId)) {
            return elemId;
        }
    }
    return InvalidEntityId;
}

void UISurfaceTouchManager::onPress(const Vec2i& pt) {
    hoveredElemId = getHoveredEntity(pt);
    if(hoveredElemId.isValid()) {
        pressElemId = hoveredElemId;
        ET_SendEvent(hoveredElemId, &ETUIInteractionBox::ET_onHover, true);
    }
}

void UISurfaceTouchManager::onMove(const Vec2i& pt) {
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

void UISurfaceTouchManager::onRelease(const Vec2i& pt) {
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

void UISurfaceTouchManager::ET_onTouch(ETouchType touchType, const Vec2i& pt) {
    switch (touchType)
    {
    case ETouchType::Press:
        onPress(pt);
        break;
    case ETouchType::Move:
        onMove(pt);
        break;
    case ETouchType::Release:
        onRelease(pt);
        break;
    default:
        break;
    }
}
