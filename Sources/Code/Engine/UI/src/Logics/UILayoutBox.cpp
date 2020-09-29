#include "Logics/UILayoutBox.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Render/ETRenderNode.hpp"
#include "UIUtils.hpp"

void UILayoutBox::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UILayoutBox>("UILayoutBox")) {
        classInfo->addField("margin", &UILayoutBox::styleMargin);
        classInfo->addField("render", &UILayoutBox::boxRenderId);
    }
}

UILayoutBox::UILayoutBox() {
}

UILayoutBox::~UILayoutBox() {
}

bool UILayoutBox::init() {
    UIElement::init();
    ETNode<ETUILayoutEvents>::connect(getEntityId());

    ET_SendEvent(getEntityId(), &ETUILayout::ET_update);

    return true;
}

void UILayoutBox::deinit() {
    UIElement::deinit();
}

void UILayoutBox::ET_onAllLogicsCreated() {
    if(boxRenderId == getEntityId()) {
        auto box = ET_getBox();
        ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, box.getSize());
        ET_SendEvent(boxRenderId, &ETRenderNode::ET_setDrawPriority, ET_getZIndex());
    }
    ET_SendEvent(getEntityId(), &ETUILayout::ET_update);
}

AABB2Di UILayoutBox::ET_getBox() const {
    return UI::GetTmScaledBox(getEntityId(), aabb);
}

UIBoxMargin UILayoutBox::ET_getMargin() const {
    return UI::CalculateMargin(getEntityId(), styleMargin);
}

void UILayoutBox::onZIndexChanged(int newZIndex) {
    ET_SendEvent(boxRenderId, &ETRenderNode::ET_setDrawPriority, newZIndex);
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

void UILayoutBox::onTransformChanged(const Transform& newTm) {
    // ET_SendEvent(getEntityId(), &ETUILayout::ET_update);
}

void UILayoutBox::ET_onLayoutChanged(const AABB2Di& newCombinedBox) {
    auto newAabb = UI::SetTmCenterToBox(getEntityId(), newCombinedBox);
    auto shiftPt = newAabb.getCenter() - newCombinedBox.getCenter();

    std::vector<EntityId> children;
    ET_SendEventReturn(children, getEntityId(), &ETUILayout::ET_getItems);
    for(auto childId : children) {
        Transform tm;
        ET_SendEventReturn(tm, childId, &ETEntity::ET_getTransform);

        tm.pt.x += static_cast<float>(shiftPt.x);
        tm.pt.y += static_cast<float>(shiftPt.y);

        ET_SendEvent(childId, &ETUIElement::ET_setIgnoreTransform, true);
        ET_SendEvent(childId, &ETEntity::ET_setTransform, tm);
        ET_SendEvent(childId, &ETUIElement::ET_setIgnoreTransform, false);
    }

    aabb = newAabb;
    updateHostLayout();
    ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, aabb.getSize());
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onBoxChanged, aabb);
}