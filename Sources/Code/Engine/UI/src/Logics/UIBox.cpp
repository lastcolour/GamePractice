#include "Logics/UIBox.hpp"
#include "UIConfig.hpp"
#include "Core/ETApplication.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Core/ETLogger.hpp"
#include "Render/ETRenderNode.hpp"

#include <cassert>

namespace {

bool isMarginChanged(const UIBoxMargin& prevMargin, const UIBoxMargin& newMargin) {
    if(prevMargin.bot != newMargin.bot) {
        return true;
    }
    if(prevMargin.top != newMargin.top) {
        return true;
    }
    if(prevMargin.left != newMargin.left) {
        return true;
    }
    if(prevMargin.right != newMargin.right) {
        return true;
    }
    return false;
}

} // namespace

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
    initBoxRender();

    return true;
}

void UIBox::deinit() {
}

AABB2Di UIBox::ET_getBox() const {
    return aabb;
}

void UIBox::calculateBox() {
    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);

    bool marginChanged = false;
    bool sizeChanged = false;

    auto newMargin = calculateMargin(tm.scale);
    if(isMarginChanged(margin, newMargin)) {
        margin = newMargin;
        marginChanged = true;
    }

    auto newBoxSize = calculateBoxSize(tm.scale);
    if(aabb.getSize() != newBoxSize) {
        aabb.bot = Vec2i(0);
        aabb.top = newBoxSize;
        sizeChanged = true;
    }

    Vec2i center = Vec2i(static_cast<int>(tm.pt.x),
        static_cast<int>(tm.pt.y));
    aabb.setCenter(center);

    if(marginChanged || sizeChanged) {
        updateLayout();
    }
    if(sizeChanged) {
        ET_SendEvent(getEntityId(), &ETUIBoxEvents::ET_onBoxResized, aabb);
        ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, aabb.getSize());
    }
}

const UIBoxStyle& UIBox::ET_getStyle() const {
    return style;
}

void UIBox::ET_setStyle(const UIBoxStyle& newStyle) {
    style = newStyle;
    calculateBox();
}

UIBoxMargin UIBox::ET_getMargin() const {
    return margin;
}

Vec2i UIBox::calculateBoxSize(const Vec3& scale) {
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
    resSize.x = static_cast<int>(resSize.x * scale.x);
    resSize.y = static_cast<int>(resSize.y * scale.y);
    return resSize;
}

UIBoxMargin UIBox::calculateMargin(const Vec3& scale) {
    UIBoxMargin resMargin;

    auto uiConfig = ET_getShared<UIConfig>();
    resMargin.left = uiConfig->getSizeOnGrind(style.margin.left);
    resMargin.right = uiConfig->getSizeOnGrind(style.margin.right);
    resMargin.bot = uiConfig->getSizeOnGrind(style.margin.bot);
    resMargin.top = uiConfig->getSizeOnGrind(style.margin.top);

    resMargin.bot = static_cast<int>(resMargin.bot * scale.y);
    resMargin.top = static_cast<int>(resMargin.top * scale.y);
    resMargin.left = static_cast<int>(resMargin.left * scale.x);
    resMargin.right = static_cast<int>(resMargin.right * scale.x);

    return resMargin;
}

void UIBox::ET_onTransformChanged(const Transform& newTm) {
    calculateBox();
}

void UIBox::ET_onRenderPortResized() {
    calculateBox();
}

void UIBox::ET_show() {
}

void UIBox::ET_hide() {
}

bool UIBox::ET_isVisible() const {
    return true;
}

void UIBox::initBoxRender() {
    if(!boxRenderId.isValid()) {
        return;
    }

    ET_SendEvent(boxRenderId, &ETRenderRect::ET_setSize, aabb.getSize());

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    ET_SendEvent(boxRenderId, &ETEntity::ET_setTransform, tm);

    ET_SendEvent(boxRenderId, &ETRenderNode::ET_setDrawPriority, ET_getZIndex());
}

void UIBox::onZIndexChanged(int newZIndex) {
    ET_SendEvent(boxRenderId, &ETRenderNode::ET_setDrawPriority, newZIndex + 1);
}