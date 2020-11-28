#include "Logics/UILayout.hpp"
#include "Reflect/ReflectContext.hpp"
#include "UI/UIBoxStyle.hpp"
#include "Entity/ETEntity.hpp"
#include "Render/ETRenderNode.hpp"
#include "Core/ETLogger.hpp"
#include "UI/ETUIViewPort.hpp"
#include "UIUtils.hpp"

#include <algorithm>
#include <cassert>

void UILayout::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UILayout>("UILayout")) {
        classInfo->addField("extraZOffset", &UILayout::extraZOffset);
        classInfo->addField("style", &UILayout::style);
        classInfo->addField("children", &UILayout::children);
    }
}

UILayout::UILayout() :
    combinedBox(Vec2i(0), Vec2i(0)),
    extraZOffset(0),
    isCalculatingLayout(false) {
}

UILayout::~UILayout() {
}

bool UILayout::init() {
    for(auto elemId : children) {
        ET_SendEvent(elemId, &ETUIElement::ET_setHostLayout, getEntityId());
    }
    calculateLayout();
    ETNode<ETUILayout>::connect(getEntityId());
    ETNode<ETUIElementEvents>::connect(getEntityId());
    ETNode<ETUIElemAligner>::connect(getEntityId());
    return true;
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
    children.push_back(entityId);
    ET_SendEvent(entityId, &ETUIElement::ET_setHostLayout, getEntityId());
    calculateLayout();
}

AABB2Di UILayout::calculateAligment(std::vector<AABB2Di>& childrenBoxes) {
    AABB2Di layoutBox;
    layoutBox.top = Vec2i(std::numeric_limits<int>::min());
    layoutBox.bot = Vec2i(std::numeric_limits<int>::max());

    for(auto& childBox : childrenBoxes) {
        layoutBox.bot.x = std::min(layoutBox.bot.x, childBox.bot.x);
        layoutBox.bot.y = std::min(layoutBox.bot.y, childBox.bot.y);
        layoutBox.top.y = std::max(layoutBox.top.y, childBox.top.y);
        layoutBox.top.x = std::max(layoutBox.top.x, childBox.top.x);
    }

    Vec2i currentCenter = layoutBox.getCenter();

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

    AABB2Di box(Vec2i(0), Vec2i(0));
    ET_SendEventReturn(box, getEntityId(), &ETUIElement::ET_getBox);

    auto center = UI::CalcAligmentCenter(style.xAlign, style.yAlign, box, layoutBox);

    Vec2i centerShift = center - layoutBox.getCenter();
    for(auto& childBox : childrenBoxes) {
        childBox.setCenter(childBox.getCenter() + centerShift);
    }

    layoutBox.setCenter(center);
    return layoutBox;
}

AABB2Di UILayout::calculateItem(int& offset, int& prevMargin, EntityId itemId) {
    AABB2Di itemBox(Vec2i(0), Vec2i(0));
    UIBoxMargin itemMargin;
    {
        bool isHidden = false;
        ET_SendEventReturn(isHidden, itemId, &ETUIElement::ET_isHidden);
        if(!isHidden) {
            ET_SendEventReturn(itemBox, itemId, &ETUIElement::ET_getBox);
            ET_SendEventReturn(itemMargin, itemId, &ETUIElement::ET_getMargin);
        }
    }

    Vec2i center(0);

    switch(style.type)
    {
    case UILayoutType::Horizontal: {
        auto marginOffset = std::max(prevMargin, itemMargin.left);
        auto xBoxSize = itemBox.getSize().x;
        center.x = offset + xBoxSize / 2 + marginOffset;
        prevMargin = itemMargin.right;
        offset += xBoxSize + marginOffset;
        break;
    }
    case UILayoutType::Vertical: {
        auto marginOffset = std::max(prevMargin, itemMargin.top);
        auto yBoxSize = itemBox.getSize().y;
        center.y = offset - yBoxSize / 2 - marginOffset;
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

    std::vector<AABB2Di> childBoxes;
    childBoxes.reserve(children.size());
    int offset = 0;
    int prevMargin = 0;
    for(auto& childId : children) {
        AABB2Di childBox;
        childBox.bot = Vec2i(0);
        childBox.top = Vec2i(0);
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

    auto childZIndex = UI::GetZIndexForChild(getEntityId());
    childZIndex += extraZOffset;

    for(size_t i = 0u; i < children.size(); ++i) {
        auto childId = children[i];
        if(!childId.isValid()) {
            continue;
        }
        auto childBox = childBoxes[i];
        auto center = childBox.getCenter();
        UI::Set2DPositionDoNotUpdateLayout(childId, center);
        ET_SendEvent(childId, &ETUIElement::ET_setZIndex, childZIndex);
    }

    ET_SendEvent(getEntityId(), &ETUILayoutEvents::ET_onLayoutChanged, combinedBox);
    isCalculatingLayout = false;
}

void UILayout::ET_onBoxChanged(const AABB2Di& newAabb) {
}

void UILayout::ET_onZIndexChanged(int newZIndex) {
    auto childZIndex = UI::GetZIndexForChild(getEntityId());
    childZIndex += extraZOffset;
    for(auto childId : children) {
        ET_SendEvent(childId, &ETUIElement::ET_setZIndex, childZIndex);
    }
}

void UILayout::ET_onAlphaChanged(float newAlpha) {
    for(auto childId : children) {
        ET_SendEvent(childId, &ETUIElement::ET_setAlpha, newAlpha);
    }
}

void UILayout::ET_onHidden(bool flag) {
    for(auto childId : children) {
        ET_SendEvent(childId, &ETUIElement::ET_setParentHidden, flag);
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

const AABB2Di& UILayout::ET_getCombinedBox() const {
    return combinedBox;
}

std::vector<EntityId> UILayout::ET_getItems() const {
    return children;
}