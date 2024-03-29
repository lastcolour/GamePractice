#include "Logics/UIBox.hpp"
#include "Render/ETRenderNode.hpp"
#include "UI/ETUILayout.hpp"
#include "UIUtils.hpp"

#include <cassert>

void UIBox::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIBox>("UIBox")) {
        classInfo->addBaseClass<UIElement>();
        classInfo->addField("style", &UIBox::style);
    }
}

UIBox::UIBox() {
}

UIBox::~UIBox() {
}

void UIBox::init() {
    UIElement::init();

    ETNode<ETUIElementGeom>::connect(getEntityId());
    ETNode<ETUIBox>::connect(getEntityId());
    ETNode<ETUIViewPortEvents>::connect(getEntityId());

    boxRenderId = getEntityId();
}

AABB2D UIBox::ET_getBox() const {
    return UI::ApplyTmToBox(getTransform(), aabb);
}

void UIBox::calculateBox() {
    auto prevBox = aabb;

    aabb.bot = Vec2(0.f);
    aabb.top = UI::CalculateBoxSize(style);

    auto& tm = getTransform();
    aabb.setCenter(tm.pt.x, tm.pt.y);

    if(prevBox.getSize() != aabb.getSize()) {
        Vec2 boxSize = aabb.getSize();
        ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, boxSize);
        updateSelfLayout();
    }

    if(prevBox.bot != aabb.bot || prevBox.top != aabb.top) {
        ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onBoxChanged, aabb);
    }
}

const UIBoxStyle& UIBox::ET_getStyle() const {
    return style;
}

void UIBox::ET_setStyle(const UIBoxStyle& newStyle) {
    style = newStyle;
    calculateBox();
    updateHostLayout();
}

UIBoxMargin UIBox::ET_getMargin() const {
    return UI::CalculateMargin(getTransform(), style.margin);
}

void UIBox::ET_onViewPortChanged(const Vec2i& newSize) {
    calculateBox();
    updateHostLayout();
}

void UIBox::onAlphaChanged(float newAlpha) {
    ET_SendEvent(boxRenderId, &ETRenderNode::ET_setAlphaMultiplier, newAlpha);
}

void UIBox::onHide(bool flag) {
    if(flag) {
        ET_SendEvent(boxRenderId, &ETRenderNode::ET_hide);
    } else {
        ET_SendEvent(boxRenderId, &ETRenderNode::ET_show);
    }
}

void UIBox::onZIndexChanged(int newZIndex) {
    ET_SendEvent(boxRenderId, &ETRenderNode::ET_setZIndex, newZIndex);
}

void UIBox::onLoaded() {
    UIElement::onLoaded();

    calculateBox();

    ET_setRenderId(boxRenderId);

    updateSelfLayout();
}

void UIBox::ET_setRenderId(EntityId newRenderId) {
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

EntityId UIBox::ET_getRenderId(EntityId newRenderId) const {
    return boxRenderId;
}