#include "UI/Logics/UIBox.hpp"
#include "UI/UIConfig.hpp"
#include "Core/ETApplication.hpp"

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

int GetSizeOnGrid(float value) {
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);
    float pixelsPerValue = renderPort.y / static_cast<float>(ET_getConfig<UIConfig>()->horizontalGrid);
    return static_cast<int>(pixelsPerValue * value);
}

} // namespace

void UIBox::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIBox>("UIBox")) {
        classInfo->addField("style", &UIBox::style);
    }
}

UIBox::UIBox() {
}

UIBox::~UIBox() {
}

bool UIBox::init() {
    calculateBox();
    ETNode<ETUIBox>::connect(getEntityId());
    ETNode<ETRenderEvents>::connect(getEntityId());
    ETNode<ETEntityEvents>::connect(getEntityId());
    return true;
}

void UIBox::deinit() {
}

const AABB2Di& UIBox::ET_getBox() const {
    return aabb;
}

void UIBox::calculateBox() {
    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);

    bool notifyLayout = false;

    auto newMargin = calculateMargin(tm.scale);
    if(isMarginChanged(margin, newMargin)) {
        margin = newMargin;
        notifyLayout = true;
    }

    auto newBoxSize = calculateBoxSize(tm.scale);
    if(aabb.getSize() != newBoxSize) {
        aabb.bot = Vec2i(0);
        aabb.top = newBoxSize;
        notifyLayout = true;
    }

    Vec2i center = Vec2i(static_cast<int>(tm.pt.x),
        static_cast<int>(tm.pt.y));
    aabb.setCenter(center);

    ET_SendEvent(getEntityId(), &ETUIBoxEvents::ET_onBoxResized);
    if(layoutId.isValid() && notifyLayout) {
        ET_SendEvent(layoutId, &ETUILayout::ET_update);
    }
}

const UIBoxStyle& UIBox::ET_getStyle() const {
    return style;
}

void UIBox::ET_setStyle(const UIBoxStyle& newStyle) {
    style = newStyle;
    calculateBox();
}

const UIBoxMargin& UIBox::ET_getMargin() const {
    return margin;
}

void UIBox::ET_setLayout(EntityId newLayoutId) {
    layoutId = newLayoutId;
}

EntityId UIBox::ET_getLayout() {
    return layoutId;
}

Vec2i UIBox::calculateBoxSize(const Vec3& scale) {
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);
    Vec2i resSize(0);
    switch (style.widthInv)
    {
    case UIBoxSizeInvariant::Grid:
        resSize.x = GetSizeOnGrid(style.width);
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
        resSize.y = GetSizeOnGrid(style.height);
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

    resMargin.left = GetSizeOnGrid(style.margin.left);
    resMargin.right = GetSizeOnGrid(style.margin.right);
    resMargin.bot = GetSizeOnGrid(style.margin.bot);
    resMargin.top = GetSizeOnGrid(style.margin.top);

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