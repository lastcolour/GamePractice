#include "Logics/UILayout.hpp"
#include "UI/UIBoxStyle.hpp"
#include "Render/ETRenderNode.hpp"
#include "UI/ETUIViewPort.hpp"
#include "UIUtils.hpp"

#include <cassert>

void UILayout::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UILayout>("UILayout")) {
        classInfo->addField("extraZOffset", &UILayout::extraZOffset);
        classInfo->addField("style", &UILayout::style);
        classInfo->addField("children", &UILayout::children);
    }
}

UILayout::UILayout() :
    combinedBox(0.f),
    extraZOffset(0),
    isCalculatingLayout(false) {
}

UILayout::~UILayout() {
}

void UILayout::init() {
    for(auto elemId : children) {
        ET_SendEvent(elemId, &ETUIElement::ET_setHostLayout, getEntityId());
    }
    calculateLayout();
    ETNode<ETUILayout>::connect(getEntityId());
    ETNode<ETUIElementEvents>::connect(getEntityId());
    ETNode<ETUIElemAligner>::connect(getEntityId());
}

void UILayout::deinit() {
    for(auto elemId : children) {
        ET_SendEvent(elemId, &ETUIElement::ET_setHostLayout, InvalidEntityId);
    }
}

const UILayoutStyle& UILayout::ET_getStyle() const {
    return style;
}

void UILayout::ET_setStyle(const UILayoutStyle& newStyle) {
    style = newStyle;
    calculateLayout();
}

void UILayout::ET_addItem(EntityId entityId) {
    if(entityId == getEntityId()) {
        LogError("[UILayout::ET_addItem] Can't add host as a item to a layout: '%s'",
            EntityUtils::GetEntityName(entityId));
        return;
    }
    children.push_back(entityId);
    UI::CopyUIElemAttribsFromParent(getEntityId(), entityId);
    ET_SendEvent(entityId, &ETUIElement::ET_setHostLayout, getEntityId());
    calculateLayout();
}

AABB2D UILayout::calculateAligment(std::vector<AABB2D>& childrenBoxes) {
    AABB2D layoutBox;
    layoutBox.top = Vec2(std::numeric_limits<float>::min());
    layoutBox.bot = Vec2(std::numeric_limits<float>::max());

    for(auto& childBox : childrenBoxes) {
        layoutBox.bot.x = std::min(layoutBox.bot.x, childBox.bot.x);
        layoutBox.bot.y = std::min(layoutBox.bot.y, childBox.bot.y);
        layoutBox.top.y = std::max(layoutBox.top.y, childBox.top.y);
        layoutBox.top.x = std::max(layoutBox.top.x, childBox.top.x);
    }

    Vec2 currentCenter = layoutBox.getCenter();

    for(auto& childBox : childrenBoxes) {
        auto aligmentCenter = UI::CalcAligmentCenter(style.xAlign, style.yAlign, layoutBox, childBox);
        switch(style.type) {
            case UILayoutType::Horizontal: {
                auto childCenter = childBox.getCenter();
                childCenter.y = aligmentCenter.y;
                childBox.setCenter(childCenter);
                break;
            }
            case UILayoutType::Vertical: {
                auto childCenter = childBox.getCenter();
                childCenter.x = aligmentCenter.x;
                childBox.setCenter(childCenter);
                break;
            }
            default: {
                assert(false && "Invalid layout type");
            }
        }
    }

    AABB2D box(0.f);
    ET_SendEventReturn(box, getEntityId(), &ETUIElement::ET_getBox);

    auto center = UI::CalcAligmentCenter(style.xAlign, style.yAlign, box, layoutBox);

    Vec2 centerShift = center - layoutBox.getCenter();
    for(auto& childBox : childrenBoxes) {
        childBox.setCenter(childBox.getCenter() + centerShift);
    }

    layoutBox.setCenter(center);
    return layoutBox;
}

AABB2D UILayout::calculateItem(float& offset, float& prevMargin, EntityId itemId) {
    AABB2D itemBox(0.f);
    UIBoxMargin itemMargin;
    {
        bool isHidden = false;
        ET_SendEventReturn(isHidden, itemId, &ETUIElement::ET_isHidden);
        if(!isHidden) {
            ET_SendEventReturn(itemBox, itemId, &ETUIElement::ET_getBox);
            ET_SendEventReturn(itemMargin, itemId, &ETUIElement::ET_getMargin);
        }
    }

    Vec2 center(0.f);

    switch(style.type)
    {
    case UILayoutType::Horizontal: {
        auto marginOffset = std::max(prevMargin, itemMargin.left);
        auto xBoxSize = itemBox.getSize().x;
        center.x = offset + xBoxSize / 2.f + marginOffset;
        prevMargin = itemMargin.right;
        offset += xBoxSize + marginOffset;
        break;
    }
    case UILayoutType::Vertical: {
        auto marginOffset = std::max(prevMargin, itemMargin.top);
        auto yBoxSize = itemBox.getSize().y;
        center.y = offset - yBoxSize / 2.f - marginOffset;
        prevMargin = itemMargin.bot;
        offset -= yBoxSize + marginOffset;
        break;
    }
    default:
        assert(false && "Invalid layout type");
    }

    itemBox.setCenter(center);

    if(style.xAlign == UIXAlign::Right) {
        itemBox.top.x += itemMargin.right;
        itemBox.bot.x -= itemMargin.right;
    } else if(style.xAlign == UIXAlign::Left) {
        itemBox.top.x += itemMargin.left;
        itemBox.bot.x -= itemMargin.left;
    }
    if(style.yAlign == UIYAlign::Top) {
        itemBox.top.y += itemMargin.top;
        itemBox.bot.y -= itemMargin.top;
    } else if(style.yAlign == UIYAlign::Bot) {
        itemBox.top.y += itemMargin.bot;
        itemBox.bot.y -= itemMargin.bot;
    }

    return itemBox;
}

void UILayout::ET_reAlign() {
    calculateLayout();
}

void UILayout::calculateLayout() {
    combinedBox = AABB2D(0.f);

    if(children.empty()) {
        ET_SendEvent(getEntityId(), &ETUILayoutEvents::ET_onLayoutChanged, combinedBox);
        return;
    }

    if(isCalculatingLayout) {
        LogError("[UILayout::calculateLayout] Can't do recursive layout calcuation for an entity '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        assert(false && "Recursive layout recalculation");
        return;
    }

    isCalculatingLayout = true;

    std::vector<AABB2D> childBoxes;
    childBoxes.reserve(children.size());
    float offset = 0.f;
    float prevMargin = 0.f;
    for(auto& childId : children) {
        AABB2D childBox(0.f);
        if(childId == getEntityId()) {
            LogWarning("[UILayout::calculateLayout] Can't have an host entity '%s' on layout", EntityUtils::GetEntityName(childId));
            childId = InvalidEntityId;
        } else if(!childId.isValid()) {
            LogWarning("[UILayout::calculateLayout] Invalid child on entity layout: '%s'", EntityUtils::GetEntityName(getEntityId()));
        } else {
            childBox = calculateItem(offset, prevMargin, childId);
        }
        childBoxes.push_back(childBox);
    }

    assert(childBoxes.size() == children.size() && "Invalid amount of child boxes");

    combinedBox = calculateAligment(childBoxes);

    int childZIndex = 0;
    ET_SendEventReturn(childZIndex, getEntityId(), &ETUIElement::ET_getZIndex);
    childZIndex += extraZOffset + 1;

    for(size_t i = 0u; i < children.size(); ++i) {
        auto childId = children[i];
        if(!childId.isValid()) {
            continue;
        }
        auto childBox = childBoxes[i];
        auto center = childBox.getCenter();
        UI::Set2DPositionDoNotUpdateLayout(childId, center);
        ET_SendEvent(childId, &ETUIElement::ET_setLayoutPos, center);
        ET_SendEvent(childId, &ETUIElement::ET_setZIndex, childZIndex);
    }

    ET_SendEvent(getEntityId(), &ETUILayoutEvents::ET_onLayoutChanged, combinedBox);
    isCalculatingLayout = false;
}

void UILayout::ET_onZIndexChanged(int newZIndex) {
    int childZIndex = newZIndex + extraZOffset + 1;
    for(auto childId : children) {
        ET_SendEvent(childId, &ETUIElement::ET_setZIndex, childZIndex);
    }
}

void UILayout::ET_onAlphaChanged(float newAlpha) {
    for(auto childId : children) {
        ET_SendEvent(childId, &ETUIElement::ET_setParentAlpha, newAlpha);
    }
}

void UILayout::ET_onHidden(bool flag) {
    for(auto childId : children) {
        ET_SendEvent(childId, &ETUIElement::ET_setParentHidden, flag);
    }
    if(!flag) {
        calculateLayout();
    }
}

void UILayout::ET_onDisabled(bool flag) {
    for(auto childId : children) {
        ET_SendEvent(childId, &ETUIElement::ET_setParentDisabled, flag);
    }
}

void UILayout::ET_onIngoreTransform(bool flag) {
    for(auto childId : children) {
        ET_SendEvent(childId, &ETUIElement::ET_setIgnoreTransform, flag);
    }
}

const AABB2D& UILayout::ET_getCombinedBox() const {
    return combinedBox;
}

std::vector<EntityId> UILayout::ET_getItems() const {
    return children;
}