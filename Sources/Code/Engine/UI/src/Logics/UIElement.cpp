#include "Logics/UIElement.hpp"

UIElement::UIElement() :
    zIndex(0) {
}

UIElement::~UIElement() {
}

bool UIElement::init() {
    ETNode<ETUIElement>::connect(getEntityId());
    ETNode<ETEntityEvents>::connect(getEntityId());

    int resZIndex = 0;
    if(getParentId() != InvalidEntityId) {
        int parentZIndex = 0;
        int parentZIndexDepth = 0;
        ET_SendEventReturn(parentZIndex, getParentId(), &ETUIElement::ET_getZIndex);
        ET_SendEventReturn(parentZIndexDepth, getParentId(), &ETUIElement::ET_getZIndexDepth);
        resZIndex = parentZIndex + parentZIndexDepth + 1;
    }
    ET_setZIndex(0);

    return true;
}

void UIElement::deinit() {
}

void UIElement::updateLayout() {
    if(layoutId.isValid()) {
        ET_SendEvent(layoutId, &ETUILayout::ET_update);
    }
}

void UIElement::ET_setLayout(EntityId newLayoutId) {
    layoutId = newLayoutId;
}

EntityId UIElement::ET_getLayout() {
    return layoutId;
}

void UIElement::ET_setZIndex(int newZIndex) {
    zIndex = newZIndex;
    int zIndexDepth = 0;
    ET_SendEventReturn(zIndexDepth, getEntityId(), &ETUIElement::ET_getZIndexDepth);
    auto childZIndex = zIndex + zIndexDepth + 1;
    std::vector<EntityId> children;
    ET_SendEventReturn(children, getEntityId(), &ETEntity::ET_getChildren);
    for(auto childId : children) {
        ET_SendEvent(childId, &ETUIElement::ET_setZIndex, childZIndex);
    }
    onZIndexChanged(zIndex);
}

int UIElement::ET_getZIndex() const {
    return zIndex;
}

int UIElement::ET_getZIndexDepth() const {
    return 1;
}

void UIElement::ET_onChildAdded(EntityId childId) {
    int zIndexDepth = ET_getZIndexDepth();
    auto childZIndex = zIndex + zIndexDepth + 1;
    ET_SendEvent(childId, &ETUIElement::ET_setZIndex, childZIndex);
}

