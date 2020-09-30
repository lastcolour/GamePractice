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

UILayoutBox::UILayoutBox()
    : aabb(Vec2i(0), Vec2i(0)) {
}

UILayoutBox::~UILayoutBox() {
}

bool UILayoutBox::init() {
    UIElement::init();
    ETNode<ETUILayoutEvents>::connect(getEntityId());

    auto combinedBox = AABB2Di(Vec2i(0), Vec2i(0));
    ET_SendEventReturn(combinedBox, getEntityId(), &ETUILayout::ET_getCombinedBox);
    ET_onLayoutChanged(combinedBox);

    return true;
}

void UILayoutBox::deinit() {
    UIElement::deinit();
}

void UILayoutBox::ET_onAllLogicsCreated() {
    auto combinedBox = AABB2Di(Vec2i(0), Vec2i(0));
    ET_SendEventReturn(combinedBox, getEntityId(), &ETUILayout::ET_getCombinedBox);
    ET_onLayoutChanged(combinedBox);
    if(boxRenderId.isValid()) {
        auto box = ET_getBox();
        ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, box.getSize());
        ET_SendEvent(boxRenderId, &ETRenderNode::ET_setDrawPriority, ET_getZIndex());
    }
}

AABB2Di UILayoutBox::ET_getBox() const {
    auto resBox = UI::GetTmScaledBox(getEntityId(), aabb);
    return UI::SetTmCenterToBox(getEntityId(), resBox);
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
    ET_SendEvent(getEntityId(), &ETUIElemAligner::ET_reAlign);
}

void UILayoutBox::ET_onLayoutChanged(const AABB2Di& newCombinedBox) {
    auto newAabb = UI::SetTmCenterToBox(getEntityId(), newCombinedBox);
    auto shiftPt = newCombinedBox.getCenter() - newAabb.getCenter();

    std::vector<EntityId> children;
    ET_SendEventReturn(children, getEntityId(), &ETUILayout::ET_getItems);
    for(auto childId : children) {
        Transform tm;
        ET_SendEventReturn(tm, childId, &ETEntity::ET_getTransform);

        tm.pt.x += static_cast<float>(shiftPt.x);
        tm.pt.y += static_cast<float>(shiftPt.y);

        UI::SetTmDoNotUpdateLayout(childId, tm);
    }

    aabb = newAabb;
    updateHostLayout();
    ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, aabb.getSize());
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onBoxChanged, aabb);
}