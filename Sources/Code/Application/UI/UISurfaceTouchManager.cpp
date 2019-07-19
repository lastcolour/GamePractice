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

void UISurfaceTouchManager::onPress(const Vec2i& pt) {
    auto interactiveElems = ET_GetAll<ETUIButton>();
    for(auto elemId : interactiveElems) {
        AABB2Di elemBox(0);
        ET_SendEventReturn(elemBox, elemId, &ETUIBox::ET_getAabb2di);
        if(pt >= elemBox.bot && pt <= elemBox.top) {
            touchedEntId = elemId;
            return;
        }
    }
}

void UISurfaceTouchManager::onMove(const Vec2i& pt) {
    if(touchedEntId.isValid()) {
    }
}

void UISurfaceTouchManager::onRelease(const Vec2i& pt) {
    if(touchedEntId.isValid()) {
        ET_SendEvent(touchedEntId, &ETUIButton::ET_onPress);
        touchedEntId = InvalidEntityId;
    }
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
