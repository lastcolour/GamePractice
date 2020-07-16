#include "Logics/UIElement.hpp"

UIElement::UIElement() :
    zIndex(0),
    isHidden(false) {
}

UIElement::~UIElement() {
}

bool UIElement::init() {
    ETNode<ETUIElement>::connect(getEntityId());
    ETNode<ETEntityEvents>::connect(getEntityId());
    onZIndexChanged(zIndex);
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
    if (layoutId == InvalidEntityId) {
        zIndex = 0;
        onZIndexChanged(zIndex);
    }
}

EntityId UIElement::ET_getLayout() {
    return layoutId;
}

void UIElement::ET_setZIndex(int newZIndex) {
    zIndex = newZIndex;
    onZIndexChanged(zIndex);
    ET_SendEvent(getEntityId(), &ETUIBoxEvents::ET_onZIndexChanged, newZIndex);
}

int UIElement::ET_getZIndex() const {
    return zIndex;
}

int UIElement::ET_getZIndexDepth() const {
    return 1;
}

void UIElement::ET_show() {
    isHidden = false;
    onHide(false);
}

void UIElement::ET_hide() {
    isHidden = true;
    onHide(true);
}

bool UIElement::ET_isHidden() const {
    return isHidden;
}