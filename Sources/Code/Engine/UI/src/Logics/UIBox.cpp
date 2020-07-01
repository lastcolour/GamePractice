#include "Logics/UIBox.hpp"
#include "UIConfig.hpp"
#include "Core/ETApplication.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Core/ETLogger.hpp"
#include "Render/ETRenderNode.hpp"

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
    ETNode<ETRenderCameraEvents>::connect(getEntityId());
    ETNode<ETUIVisibleElement>::connect(getEntityId());

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

    margin = calculateMargin();
    aabb.bot = Vec2i(0);
    aabb.top = calculateBoxSize();

    Vec2i center = Vec2i(static_cast<int>(tm.pt.x),
        static_cast<int>(tm.pt.y));
    aabb.setCenter(center);

    ET_SendEvent(getEntityId(), &ETUIBoxEvents::ET_onBoxResized, aabb);
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

    const auto& scale = tm.scale;
    auto resMargin = margin;

    resMargin.bot = static_cast<int>(resMargin.bot * scale.y);
    resMargin.top = static_cast<int>(resMargin.top * scale.y);
    resMargin.left = static_cast<int>(resMargin.left * scale.x);
    resMargin.right = static_cast<int>(resMargin.right * scale.x);

    return resMargin;
}

Vec2i UIBox::calculateBoxSize() {
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);
    Vec2i resSize(0);

    auto uiConfig = ET_getShared<UIConfig>();
    switch (style.widthInv)
    {
    case UIBoxSizeInvariant::Grid:
        resSize.x = uiConfig->getSizeOnGrind(style.width);
        break;
    case UIBoxSizeInvariant::Relative:
        resSize.x = static_cast<int>(renderPort.x * style.width);
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
        resSize.y = static_cast<int>(renderPort.y * style.height);
        break;
    default:
        assert(false && "Invalid size invariant");
    }
    return resSize;
}

UIBoxMargin UIBox::calculateMargin() {
    UIBoxMargin resMargin;

    auto uiConfig = ET_getShared<UIConfig>();
    resMargin.left = uiConfig->getSizeOnGrind(style.margin.left);
    resMargin.right = uiConfig->getSizeOnGrind(style.margin.right);
    resMargin.bot = uiConfig->getSizeOnGrind(style.margin.bot);
    resMargin.top = uiConfig->getSizeOnGrind(style.margin.top);

    return resMargin;
}

void UIBox::ET_onRenderPortResized() {
    calculateBox();
    updateLayout();
    ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, aabb.getSize());
}

void UIBox::ET_show() {
}

void UIBox::ET_hide() {
}

bool UIBox::ET_isVisible() const {
    return true;
}

void UIBox::onZIndexChanged(int newZIndex) {
    ET_SendEvent(boxRenderId, &ETRenderNode::ET_setDrawPriority, newZIndex);
    ET_SendEvent(getEntityId(), &ETUILayout::ET_update);
}

void UIBox::ET_onAllLogicsCreated() {
    if(boxRenderId == getEntityId()) {
        ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, aabb.getSize());
        ET_SendEvent(boxRenderId, &ETRenderNode::ET_setDrawPriority, ET_getZIndex());
    }
    ET_SendEvent(getEntityId(), &ETUILayout::ET_update);
}

void UIBox::ET_onTransformChanged(const Transform& newTm) {
    calculateBox();
}