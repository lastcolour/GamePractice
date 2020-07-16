#include "Logics/UIElement.hpp"

UIElement::UIElement() :
    alpha(1.f),
    zIndex(0),
    isIgnoringTransform(false),
    isHidden(false),
    isEnabled(true) {
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
        ET_setZIndex(0);
    }
}

EntityId UIElement::ET_getLayout() {
    return layoutId;
}

void UIElement::ET_setZIndex(int newZIndex) {
    zIndex = newZIndex;
    onZIndexChanged(zIndex);
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onZIndexChanged, newZIndex);
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
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onHidden, isHidden);
    if(isEnabled) {
        onDisabled(false);
        ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onDisabled, false);
    }
}

void UIElement::ET_hide() {
    isHidden = true;
    onHide(true);
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onHidden, isHidden);
    onDisabled(true);
    if(isEnabled) {
        ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onDisabled, true);
    }
}

bool UIElement::ET_isHidden() const {
    return isHidden;
}

float UIElement::ET_getAlpha() const {
    return alpha;
}
void UIElement::ET_setAlpha(float newAlpha) {
    alpha = newAlpha;
    onAlphaChanged(alpha);
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onAlphaChanged, newAlpha);
}

void UIElement::ET_enable() {
    isEnabled = true;
    onDisabled(!isEnabled);
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onDisabled, !isEnabled);
}

void UIElement::ET_disable() {
    isEnabled = false;
    onDisabled(!isEnabled);
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onDisabled, !isEnabled);
}

bool UIElement::ET_isEnabled() const {
    return isEnabled;
}

void UIElement::ET_setIgnoreTransform(bool flag) {
    isIgnoringTransform = flag;
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onIngoreTransform, isIgnoringTransform);
}

void UIElement::ET_onTransformChanged(const Transform& newTm) {
    if(isIgnoringTransform) {
        return;
    }
    onTransformChanged(newTm);
}