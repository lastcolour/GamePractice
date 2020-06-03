#include "UI/Logics/UILayout.hpp"
#include "Reflect/ReflectContext.hpp"
#include "UI/Logics/UIBoxStyle.hpp"
#include "Entity/ETEntityInterfaces.hpp"

#include <algorithm>
#include <cassert>

void UILayout::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UILayout>("UILayout")) {
        classInfo->addField("style", &UILayout::style);
        classInfo->addField("children", &UILayout::children);
    }
}

UILayout::UILayout() {
}

UILayout::~UILayout() {
}

bool UILayout::init() {
    calculateLayout();
    ETNode<ETUILayout>::connect(getEntityId());
    ETNode<ETUIBoxEvents>::connect(getEntityId());
    return true;
}

void UILayout::deinit() {
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
    ET_SendEvent(entityId, &ETUIBox::ET_setLayout, getEntityId());
    calculateLayout();
}

void UILayout::ET_onBoxResized() {
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
    ET_SendEventReturn(box, getEntityId(), &ETUIBox::ET_getBox);
    auto center = calcAligmentCenter(box, layoutBox);

    Vec2i centerShift = center - layoutBox.getCenter();
    for(auto& childBox : childrenBoxes) {
        childBox.setCenter(childBox.getCenter() + centerShift);
    }
}

AABB2Di UILayout::calculateItem(Vec2i& offset, Vec2i& prevMargin, EntityId itemId) {
    AABB2Di itemBox;
    ET_SendEventReturn(itemBox, itemId, &ETUIBox::ET_getBox);
    UIBoxMargin itemMargin;
    ET_SendEventReturn(itemMargin, itemId, &ETUIBox::ET_getMargin);

    Vec2i center(0);

    switch(style.type)
    {
    case UILayoutType::Horizontal: {
        center.x = offset.x + itemBox.getSize().x / 2 + std::max(prevMargin.x, itemMargin.left);
        prevMargin.x = itemMargin.right;
        offset.x += itemBox.getSize().x;
        break;
    }
    case UILayoutType::Vertical: {
        center.y = offset.y - itemBox.getSize().y / 2 - std::max(prevMargin.y, itemMargin.top);
        prevMargin.y = itemMargin.bot;
        offset.y -= itemBox.getSize().y;
        break;
    }
    default:
        assert(false && "Invalid layout type");
    }

    itemBox.setCenter(center);
    return itemBox;
}

void UILayout::ET_update() {
    calculateLayout();
}

void UILayout::calculateLayout() {
    if(children.empty()) {
        return;
    }
    std::vector<AABB2Di> childBoxes;
    childBoxes.reserve(children.size());
    Vec2i offset(0);
    Vec2i prevMargin(0);
    for(auto childId : children) {
        auto resBox = calculateItem(offset, prevMargin, childId);
        childBoxes.push_back(resBox);
    }

    calculateAligment(childBoxes);

    for(size_t i = 0u; i < children.size(); ++i) {
        auto childId = children[i];
        auto childBox = childBoxes[i];

        Transform tm;
        ET_SendEventReturn(tm, childId, &ETEntity::ET_getTransform);

        auto center = childBox.getCenter();
        tm.pt.x = static_cast<float>(center.x);
        tm.pt.y = static_cast<float>(center.y);

        ET_SendEvent(childId, &ETEntity::ET_setTransform, tm);
    }
}
