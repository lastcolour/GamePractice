#include "UI/Logics/UIList.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include <cassert>
#include <algorithm>
#include <limits>

UIList::UIList() :
    listType(UIListType::Vertical) {
}

UIList::~UIList() {
}

void UIList::ET_addChildElement(EntityId newElemId) {
    ET_SendEvent(getEntityId(), &ETGameObject::ET_addChild, newElemId);
    ET_SendEvent(newElemId, &ETUIBox::ET_boxResizeInside, getParentAaabb2di());

    const auto& currBox = ET_getAabb2di();
    int offset = 0;
    AABB2Di elemBox;
    if(!children.empty()) {
        ET_SendEventReturn(elemBox, children.back(), &ETUIBox::ET_getAabb2di);
        if(listType == UIListType::Horizontal) {
            offset = elemBox.top.x;
        } else {
            offset = elemBox.bot.y;
        }
    } else {
        if(listType == UIListType::Horizontal) {
            offset = currBox.top.x;
        } else {
            offset = currBox.bot.y;
        }
    }
    elemBox = AABB2Di(0);
    ET_SendEventReturn(elemBox, newElemId, &ETUIBox::ET_getAabb2di);
    elemBox.setCenter(elemBox.getSize() / 2);

    Vec2i center = elemBox.getCenter();
    if(listType == UIListType::Horizontal) {
        center.x += + offset;
    } else {
        center.y = offset - center.y;
    }

    ET_SendEvent(newElemId, &ETUIBox::ET_setCenter, center);
    children.push_back(newElemId);

    calcResListBox();
}

void UIList::calcResListBox() {
    AABB2Di listBox;
    listBox.top = Vec2i(std::numeric_limits<int>::min());
    listBox.bot = Vec2i(std::numeric_limits<int>::max());
    for(auto entId : children) {
        AABB2Di childBox = AABB2Di(0);
        ET_SendEventReturn(childBox, entId, &ETUIBox::ET_getAabb2di);
        auto center = childBox.getCenter();
        auto size = childBox.getSize();

        if(listType == UIListType::Horizontal) {
            Vec2i pt(center.x, size.y / 2);
            childBox.setCenter(pt);
        } else {
            Vec2i pt(size.x / 2, center.y);
            childBox.setCenter(pt);
        }
        listBox.top.x = std::max(listBox.top.x, childBox.top.x);
        listBox.top.y = std::max(listBox.top.y, childBox.top.y);
        listBox.bot.x = std::min(listBox.bot.x, childBox.bot.x);
        listBox.bot.y = std::min(listBox.bot.y, childBox.bot.y);
        ET_SendEvent(entId, &ETUIBox::ET_setCenter, childBox.getCenter());
    }

    setBox(listBox);
    ET_alignInBox(getParentAaabb2di());

    const auto& currBox = ET_getAabb2di();
    const auto center = currBox.getCenter();
    int offset = currBox.bot.x;
    if(listType == UIListType::Vertical) {
        offset = currBox.top.y;
    }

    for(auto entId : children) {
        AABB2Di childBox(0);
        ET_SendEventReturn(childBox, entId, &ETUIBox::ET_getAabb2di);
        Vec2i childCenter(0);
        Vec2i childSize = childBox.getSize();
        if(listType == UIListType::Horizontal) {
            childCenter = Vec2i(offset + childSize.x / 2, center.y);
            offset += childSize.x;
        } else {
            childCenter = Vec2i(center.x, offset - childSize.y / 2);
            offset -= childSize.y;
        }

        ET_SendEvent(entId, &ETUIBox::ET_setCenter, childCenter);
    }
}

bool UIList::serialize(const JSONNode& node) {
    if(!UIBox::serialize(node)) {
        LogWarning("[UIList::serialize] UIBox searilization failed");
        return false;
    }
    auto childrenNode = node.object("children");
    if(!childrenNode) {
        LogWarning("[UIList::serialize] Can't find required children node");
        return false;
    }
    if(!childrenNode.size()) {
        LogWarning("[UIList::serialize] Empty children list");
        return false;
    }
    std::string list("vert");
    node.value("type", list);
    if(list == "vert") {
        listType = UIListType::Vertical;
    } else if(list == "horz") {
        listType = UIListType::Horizontal;
    } else {
        LogWarning("[UIList::serialize] Invalid list type: %s", list);
        return false;
    }
    for(auto& childNode : childrenNode) {
        std::string childObjName;
        childNode.value(childObjName);
        EntityId childEntId;
        ET_SendEventReturn(childEntId, &ETGameObjectManager::ET_createGameObject, childObjName.c_str());
        if(childEntId != InvalidEntityId) {
            children.push_back(childEntId);
        }
    }
    return true;
}

void UIList::calcList() {
    std::vector<EntityId> elems = std::move(children);
    for(auto entId : elems) {
        ET_addChildElement(entId);
    }
}

void UIList::ET_setType(UIListType newListType) {
    listType = newListType;
    calcList();
}

void UIList::ET_boxResize() {
    UIBox::ET_boxResize();
    calcList();
}

bool UIList::init() {
    if(!UIBox::init()) {
        LogWarning("[UIList::init] UIBox init failed");
        return false;
    }
    calcList();
    ETNode<ETUIList>::connect(getEntityId());
    return true;
}