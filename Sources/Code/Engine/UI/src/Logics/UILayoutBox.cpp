#include "Logics/UILayoutBox.hpp"
#include "Render/ETRenderNode.hpp"
#include "UIUtils.hpp"

void UILayoutBox::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UILayoutBox>("UILayoutBox")) {
        classInfo->addBaseClass<UIElement>();
        classInfo->addField("margin", &UILayoutBox::styleMargin);
    }
}

UILayoutBox::UILayoutBox() :
    aabb(0.f) {
}

UILayoutBox::~UILayoutBox() {
}

void UILayoutBox::init() {
    UIElement::init();

    ETNode<ETUIElementGeom>::connect(getEntityId());
    ETNode<ETUILayoutEvents>::connect(getEntityId());

    boxRenderId = getEntityId();
}

void UILayoutBox::deinit() {
    UIElement::deinit();
}

void UILayoutBox::onLoaded() {
    UIElement::onLoaded();

    ET_setRenderId(boxRenderId);

    updateSelfLayout();
}

void UILayoutBox::ET_setRenderId(EntityId newRenderId) {
    boxRenderId = newRenderId;
    if(!boxRenderId.isValid()) {
        return;
    }
    Vec2 boxSize = aabb.getSize();
    ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, boxSize);
    ET_SendEvent(boxRenderId, &ETRenderNode::ET_setZIndex, ET_getZIndex());
    if(ET_isHidden()) {
        ET_SendEvent(boxRenderId, &ETRenderNode::ET_hide);
    } else {
        ET_SendEvent(boxRenderId, &ETRenderNode::ET_show);
    }
}

EntityId UILayoutBox::ET_getRenderId(EntityId newRenderId) const {
    return boxRenderId;
}

AABB2D UILayoutBox::ET_getBox() const {
    return UI::ApplyTmToBox(getTransform(), aabb);
}

UIBoxMargin UILayoutBox::ET_getMargin() const {
    return UI::CalculateMargin(getTransform(), styleMargin);
}

void UILayoutBox::onZIndexChanged(int newZIndex) {
    ET_SendEvent(boxRenderId, &ETRenderNode::ET_setZIndex, newZIndex);
}

void UILayoutBox::onHide(bool flag) {
    if(flag) {
        ET_SendEvent(boxRenderId, &ETRenderNode::ET_hide);
    } else {
        ET_SendEvent(boxRenderId, &ETRenderNode::ET_show);
    }
}

void UILayoutBox::onAlphaChanged(float newAlpha) {
    ET_SendEvent(boxRenderId, &ETRenderNode::ET_setAlphaMultiplier, newAlpha);
}

void UILayoutBox::ET_onLayoutChanged(const AABB2D& newCombinedBox) {
    AABB2D prevAabb = aabb;
    aabb = newCombinedBox;

    auto& tm = getTransform();
    aabb.setCenter(tm.pt.x, tm.pt.y);

    Vec2 newSize = aabb.getSize();

    if(prevAabb.getSize() != newSize) {
        ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, newSize);
    }
    if(prevAabb.bot != aabb.bot || prevAabb.top != aabb.top) {
        ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onBoxChanged, aabb);
    }
}