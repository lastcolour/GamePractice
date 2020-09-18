#include "Logics/UIBox.hpp"
#include "UIConfig.hpp"
#include "Core/ETApplication.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Core/ETLogger.hpp"
#include "Render/ETRenderNode.hpp"
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

    if(ET_IsExistNode<ETRenderRect>(boxRenderId)) {
        ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, aabb.getSize());
    }
    return true;
}

void UIBox::deinit() {
}

AABB2Di UIBox::ET_getBox() const {
    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    const auto& scale = tm.scale;

    AABB2Di resBox;
    resBox.bot = Vec2i(0);
    resBox.top = aabb.getSize();
    resBox.top.x = static_cast<int>(resBox.top.x * scale.x);
    resBox.top.y = static_cast<int>(resBox.top.y * scale.y);
    resBox.setCenter(aabb.getCenter());

    return resBox;
}

void UIBox::calculateBox() {
    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);

    aabb.bot = Vec2i(0);
    aabb.top = calculateBoxSize();

    Vec2i center = Vec2i(static_cast<int>(tm.pt.x),
        static_cast<int>(tm.pt.y));
    aabb.setCenter(center);

    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onBoxResized, aabb);
    ET_SendEvent(getEntityId(), &ETUILayout::ET_update);
}

const UIBoxStyle& UIBox::ET_getStyle() const {
    return style;
}

void UIBox::ET_setStyle(const UIBoxStyle& newStyle) {
    style = newStyle;
    calculateBox();
    updateLayout();
    ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, aabb.getSize());
}

UIBoxMargin UIBox::ET_getMargin() const {
    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    return UI::CalculateMargin(style.margin, tm);
}

Vec2i UIBox::calculateBoxSize() {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    Vec2i resSize(0);

    auto uiConfig = ET_getShared<UIConfig>();
    switch (style.widthInv)
    {
    case UIBoxSizeInvariant::Grid:
        resSize.x = uiConfig->getSizeOnGrind(style.width);
        break;
    case UIBoxSizeInvariant::Relative:
        resSize.x = static_cast<int>(viewPort.x * style.width);
        break;
    default:
        assert(false && "Invalid size invariant");
    }
    switch (style.heightInv)
    {
    case UIBoxSizeInvariant::Grid:
        resSize.y = uiConfig->getSizeOnGrind(style.height);
        break;
    case UIBoxSizeInvariant::Relative:
        resSize.y = static_cast<int>(viewPort.y * style.height);
        break;
    default:
        assert(false && "Invalid size invariant");
    }
    return resSize;
}

void UIBox::ET_onViewPortChanged(const Vec2i& newSize) {
    calculateBox();
    updateLayout();
    ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, aabb.getSize());
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

void UIBox::ET_onAllLogicsCreated() {
    if(boxRenderId == getEntityId()) {
        ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, aabb.getSize());
        ET_SendEvent(boxRenderId, &ETRenderNode::ET_setDrawPriority, ET_getZIndex());
    }
    ET_SendEvent(getEntityId(), &ETUILayout::ET_update);
}

void UIBox::onTransformChanged(const Transform& newTm) {
    calculateBox();
}