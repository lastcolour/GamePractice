#include "Logics/UILayoutBox.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Render/ETRenderNode.hpp"
#include "UIUtils.hpp"

void UILayoutBox::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UILayoutBox>("UILayoutBox")) {
        classInfo->addBaseClass<UIElement>();
        classInfo->addField("margin", &UILayoutBox::styleMargin);
    }
}

UILayoutBox::UILayoutBox() :
    aabb(Vec2i(0), Vec2i(0)) {
}

UILayoutBox::~UILayoutBox() {
}

void UILayoutBox::init() {
    UIElement::init();
    ETNode<ETUILayoutEvents>::connect(getEntityId());

    boxRenderId = getEntityId();
}

void UILayoutBox::deinit() {
    UIElement::deinit();
}

void UILayoutBox::ET_onLoaded() {
    UIElement::ET_onLoaded();
    ET_SendEvent(getEntityId(), &ETUIElemAligner::ET_reAlign);

    auto box = ET_getBox();
    Vec2i boxSize = box.getSize();
    ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, Vec2(boxSize.x, boxSize.y));
    ET_SendEvent(boxRenderId, &ETRenderNode::ET_setDrawPriority, ET_getZIndex());
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

void UILayoutBox::ET_onLayoutChanged(const AABB2Di& newCombinedBox) {
    auto newAabb = UI::SetTmCenterToBox(getEntityId(), newCombinedBox);
    auto shiftPt = newAabb.getCenter() - newCombinedBox.getCenter();

    if(shiftPt.x != 0 || shiftPt.y != 0) {
        std::vector<EntityId> children;
        ET_SendEventReturn(children, getEntityId(), &ETUILayout::ET_getItems);
        for(auto childId : children) {
            Transform tm;
            ET_SendEventReturn(tm, childId, &ETEntity::ET_getTransform);

            tm.pt.x += static_cast<float>(shiftPt.x);
            tm.pt.y += static_cast<float>(shiftPt.y);

            UI::SetTMDoNotUpdateLayout(childId, tm);
        }
    }

    aabb = newAabb;
    updateHostLayout();
    Vec2i boxSize = aabb.getSize();
    ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, Vec2(boxSize.x, boxSize.y));
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onBoxChanged, aabb);
}