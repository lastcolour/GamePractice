#include "UI/UISurfaceTouchManager.hpp"
#include "UI/UIETInterfaces.hpp"

UISurfaceTouchManager::UISurfaceTouchManager() {
}

UISurfaceTouchManager::~UISurfaceTouchManager() {
}

bool UISurfaceTouchManager::init() {
    ETNode<ETSurfaceEvents>::connect(getEntityId());
    return true;
}

void UISurfaceTouchManager::deinit() {
    ETNode<ETSurfaceEvents>::disconnect();
}

bool UISurfaceTouchManager::isHover(const Vec2i& pt, EntityId entId) const {
    AABB2Di elemBox(0);
    ET_SendEventReturn(elemBox, entId, &ETUIBox::ET_getAabb2di);
    return pt >= elemBox.bot && pt <= elemBox.top;
}

EntityId UISurfaceTouchManager::getHoveredEntity(const Vec2i& pt) const {
    auto interactiveElems = ET_GetAll<ETUIButton>();
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
        ET_SendEvent(hoveredElemId, &ETUIButton::ET_onHover, true);
    }
}

void UISurfaceTouchManager::onMove(const Vec2i& pt) {
    auto currHoveredId = getHoveredEntity(pt);
    if (currHoveredId == InvalidEntityId) {
        if(hoveredElemId.isValid()) {
            ET_SendEvent(hoveredElemId, &ETUIButton::ET_onHover, false);
            hoveredElemId = InvalidEntityId;
        }
    } else if (currHoveredId != hoveredElemId) {
        if(hoveredElemId.isValid()) {
            ET_SendEvent(hoveredElemId, &ETUIButton::ET_onHover, false);
        }
        ET_SendEvent(currHoveredId, &ETUIButton::ET_onHover, true);
        hoveredElemId = currHoveredId;
    }
}

void UISurfaceTouchManager::onRelease(const Vec2i& pt) {
    auto releseElemId = getHoveredEntity(pt);
    if (hoveredElemId.isValid()) {
        ET_SendEvent(hoveredElemId, &ETUIButton::ET_onHover, false);
        if(releseElemId == pressElemId) {
            ET_SendEvent(hoveredElemId, &ETUIButton::ET_onPress);
        }
    }
    hoveredElemId = InvalidEntityId;
    pressElemId = InvalidEntityId;
}

void UISurfaceTouchManager::ET_onSurfaceTouch(ETouchType touchType, const Vec2i& pt) {
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
