#include "Logics/UILayout.hpp"
#include "Reflect/ReflectContext.hpp"
#include "UI/UIBoxStyle.hpp"
#include "Entity/ETEntity.hpp"
#include "Render/ETRenderNode.hpp"
#include "Core/ETLogger.hpp"

#include <algorithm>
#include <cassert>

void UILayout::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UILayout>("UILayout")) {
        classInfo->addField("style", &UILayout::style);
        classInfo->addField("children", &UILayout::children);
    }
}

UILayout::UILayout() :
    isCalculatingLayout(false) {
}

UILayout::~UILayout() {
}

bool UILayout::init() {
    for(auto elemId : children) {
        ET_SendEvent(elemId, &ETUIElement::ET_setLayout, getEntityId());
    }
    calculateLayout();
    ETNode<ETUILayout>::connect(getEntityId());
    ETNode<ETUIElementEvents>::connect(getEntityId());
    return true;
}

void UILayout::deinit() {
    for(auto elemId : children) {
        ET_SendEvent(elemId, &ETUIElement::ET_setLayout, InvalidEntityId);
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
    ET_SendEvent(entityId, &ETUIElement::ET_setLayout, getEntityId());
    calculateLayout();
}

Vec2i UILayout::calcAligmentCenter(AABB2Di& parentBox, AABB2Di& box) {
    Vec2i center(0);
    switch(style.xAlign) {
        case UIXAlign::Left: {
            center.x = parentBox.bot.x + box.getSize().x / 2;
            break;
        }
        case UIXAlign::Center: {
            center.x = parentBox.getCenter().x;
            break;
        }
        case UIXAlign::Right: {
            center.x = parentBox.top.x - box.getSize().x / 2;
            break;
        }
        default: {
            assert(false && "Invalid x-aligment type");
        }
    }
    switch(style.yAlign) {
        case UIYAlign::Bot: {
            center.y = parentBox.bot.y + box.getSize().y / 2;
            break;
        }
        case UIYAlign::Center: {
            center.y = parentBox.getCenter().y;
            break;
        }
        case UIYAlign::Top: {
            center.y = parentBox.top.y - box.getSize().y / 2;
            break;
        }
        default: {
            assert(false && "Invalid y-aligment type");
        }
    }
    return center;
}

void UILayout::calculateAligment(std::vector<AABB2Di>& childrenBoxes) {
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
        auto aligmentCenter = calcAligmentCenter(layoutBox, childBox);
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

    AABB2Di box;
    ET_SendEventReturn(box, getEntityId(), &ETUIElement::ET_getBox);
    auto center = calcAligmentCenter(box, layoutBox);

    Vec2i centerShift = center - layoutBox.getCenter();
    for(auto& childBox : childrenBoxes) {
        childBox.setCenter(childBox.getCenter() + centerShift);
    }
}

AABB2Di UILayout::calculateItem(int& offset, int& prevMargin, EntityId itemId) {
    AABB2Di itemBox;
    ET_SendEventReturn(itemBox, itemId, &ETUIElement::ET_getBox);
    UIBoxMargin itemMargin;
    ET_SendEventReturn(itemMargin, itemId, &ETUIElement::ET_getMargin);

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

void UILayout::ET_update() {
    calculateLayout();
}

void UILayout::calculateLayout() {
    if(children.empty()) {
        return;
    }

    if(isCalculatingLayout) {
        LogError("[UILayout::calculateLayout] Can't do recursive layout calcuation for an entity '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        assert(false && "Invalid request to calculate layout");
        return;
    }

    isCalculatingLayout = true;

    std::vector<AABB2Di> childBoxes;
    childBoxes.reserve(children.size());
    int offset = 0;
    int prevMargin = 0;
    for(auto childId : children) {
        AABB2Di childBox;
        childBox.bot = Vec2i(0);
        childBox.top = Vec2i(0);
        if(childId == getEntityId()) {
            LogWarning("[UILayout::calculateLayout] Can't have an host entity '%s' on layout", EntityUtils::GetEntityName(childId));
        } else if(!childId.isValid()) {
            LogWarning("[UILayout::calculateLayout] Invalid child on entity layout: '%s'", EntityUtils::GetEntityName(getEntityId()));
        } else {
            childBox = calculateItem(offset, prevMargin, childId);
        }
        childBoxes.push_back(childBox);
    }

    assert(childBoxes.size() == children.size() && "Invalid amount of child boxes");

    calculateAligment(childBoxes);

    int zIndex = 0;
    int zIndexDepth = 0;
    ET_SendEventReturn(zIndex, getEntityId(), &ETUIElement::ET_getZIndex);
    ET_SendEventReturn(zIndexDepth, getEntityId(), &ETUIElement::ET_getZIndexDepth);
    int childZIndex = zIndex + zIndexDepth + 1;

    for(size_t i = 0u; i < children.size(); ++i) {
        auto childId = children[i];
        if(!childId.isValid() || childId == getEntityId()) {
            continue;
        }

        auto childBox = childBoxes[i];

        Transform tm;
        ET_SendEventReturn(tm, childId, &ETEntity::ET_getTransform);

        auto center = childBox.getCenter();
        tm.pt.x = static_cast<float>(center.x);
        tm.pt.y = static_cast<float>(center.y);

        ET_SendEvent(childId, &ETEntity::ET_setTransform, tm);

        ET_SendEvent(childId, &ETUIElement::ET_setZIndex, childZIndex);
        ET_SendEvent(childId, &ETRenderNode::ET_setDrawPriority, childZIndex);
    }

    isCalculatingLayout = false;
}

void UILayout::ET_onBoxResized(const AABB2Di& newAabb) {
    calculateLayout();
}

void UILayout::ET_onZIndexChanged(int newZIndex) {
    int zIndex = 0;
    int zIndexDepth = 0;
    ET_SendEventReturn(zIndex, getEntityId(), &ETUIElement::ET_getZIndex);
    ET_SendEventReturn(zIndexDepth, getEntityId(), &ETUIElement::ET_getZIndexDepth);
    int childZIndex = zIndex + zIndexDepth + 1;

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
    if(flag) {
        for(auto childId : children) {
            ET_SendEvent(childId, &ETUIElement::ET_hide);
        }
    } else {
        for(auto childId : children) {
            ET_SendEvent(childId, &ETUIElement::ET_show);
        }
    }
}

void UILayout::ET_onDisabled(bool flag) {
    if(flag) {
        for(auto childId : children) {
            ET_SendEvent(childId, &ETUIElement::ET_disable);
        }
    } else {
        for(auto childId : children) {
            ET_SendEvent(childId, &ETUIElement::ET_enable);
        }
    }
}

void UILayout::ET_onIngoreTransform(bool flag) {
    for(auto childId : children) {
        ET_SendEvent(childId, &ETUIElement::ET_setIgnoreTransform, flag);
    }
}