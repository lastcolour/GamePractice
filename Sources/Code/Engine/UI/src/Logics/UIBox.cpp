#include "Logics/UIBox.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Core/ETLogger.hpp"
#include "Render/ETRenderNode.hpp"
#include "UI/ETUILayout.hpp"
#include "UIUtils.hpp"

#include <cassert>

void UIBox::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIBox>("UIBox")) {
        classInfo->addField("style", &UIBox::style);
        classInfo->addField("render", &UIBox::boxRenderId);
    }
}

UIBox::UIBox() {
}

UIBox::~UIBox() {
}

bool UIBox::init() {
    UIElement::init();

    ETNode<ETUIBox>::connect(getEntityId());
    ETNode<ETUIViewPortEvents>::connect(getEntityId());

    calculateBox();

    return true;
}

void UIBox::deinit() {
    UIElement::deinit();
}

AABB2Di UIBox::ET_getBox() const {
    auto resBox = UI::SetTmCenterToBox(getEntityId(), aabb);
    return UI::GetTmScaledBox(getEntityId(), resBox);
}

void UIBox::calculateBox() {
    auto prevBox = aabb;

    aabb.bot = Vec2i(0);
    aabb.top = UI::CalculateBoxSize(style);
    aabb = UI::SetTmCenterToBox(getEntityId(), aabb);

    if(prevBox.getSize() != aabb.getSize()) {
        Vec2i boxSize = aabb.getSize();
        ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, Vec2(boxSize.x, boxSize.y));
        ET_SendEvent(getEntityId(), &ETUIElemAligner::ET_reAlign);
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
    return UI::CalculateMargin(getEntityId(), style.margin);
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
    ET_SendEvent(boxRenderId, &ETRenderNode::ET_setDrawPriority, newZIndex);
}

void UIBox::ET_onLoaded() {
    if(boxRenderId.isValid()) {
        Vec2i boxSize = aabb.getSize();
        ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, Vec2(boxSize.x, boxSize.y));
        ET_SendEvent(boxRenderId, &ETRenderNode::ET_setDrawPriority, ET_getZIndex());
    }
    ET_SendEvent(getEntityId(), &ETUIElemAligner::ET_reAlign);
}

void UIBox::onTransformChanged(const Transform& newTm) {
    calculateBox();
}