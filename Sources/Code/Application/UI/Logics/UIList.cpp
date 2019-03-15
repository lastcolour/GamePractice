#include "UI/Logics/UIList.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include <cassert>
#include <algorithm>

UIList::UIList() :
    listType(ListType::Vertical) {
}

UIList::~UIList() {
}

void  UIList::ET_addElement(EntityId newElemId) {
    const auto& box = ET_getAaabb2di();
    Vec2i offset = Vec2i(0, box.top.y);
    AABB2Di elemBox;
    if(!children.empty()) {
        ET_SendEventReturn(elemBox, children.back(), &ETUIBox::ET_getAaabb2di);
        if(listType == ListType::Horizontal) {
            offset.x = elemBox.top.x;
        } else {
            offset.y = elemBox.bot.y;
        }
    }
    elemBox = AABB2Di(0);
    ET_SendEventReturn(elemBox, newElemId, &ETUIBox::ET_getAaabb2di);
    elemBox.setCenter(elemBox.getSize() / 2);
    Vec2i center = elemBox.getCenter();
    if(listType == ListType::Horizontal) {
        center = Vec2i(center.x + offset.x, center.y); 
    } else {
        center = Vec2i(center.x, offset.y - center.y);
    }

    ET_SendEvent(getEntityId(), &ETGameObject::ET_addChild, newElemId);
    ET_SendEvent(newElemId, &ETUIBox::ET_setCenter, center);
    children.push_back(newElemId);

    calcResListBox();
}

void UIList::calcResListBox() {
    AABB2Di listBox(0);
    for(auto entId : children) {
        AABB2Di childBox = AABB2Di(0);
        ET_SendEventReturn(childBox, entId, &ETUIBox::ET_getAaabb2di);
        auto center = childBox.getCenter();
        auto size = childBox.getSize();

        if(listType == ListType::Horizontal) {
            Vec2i pt(center.x, size.y / 2);
            childBox.setCenter(pt);
        } else {
            Vec2i pt(size.x / 2, center.y);
            childBox.setCenter(pt);
        }
        listBox.top.x = std::max(listBox.top.x, childBox.top.x);
        listBox.top.y = std::max(listBox.top.y, childBox.top.y);
        listBox.bot.x = std::min(listBox.bot.x, childBox.bot.x);
        listBox.bot.y = std::min(listBox.bot.y, childBox.bot.x);
        ET_SendEvent(entId, &ETUIBox::ET_setCenter, childBox.getCenter());
    }

    setBox(listBox);
    ET_alignInBox(ET_getParentAaabb2di());
    const auto& box = ET_getAaabb2di();

    for(auto entId : children) {
        AABB2Di childBox = AABB2Di(0);
        ET_SendEventReturn(childBox, entId, &ETUIBox::ET_getAaabb2di);
        if(listType == ListType::Horizontal) {
            childBox = childBox + Vec2i(0, box.bot.y);
        } else {
            childBox = childBox + Vec2i(box.bot.x, 0);
        }
        ET_SendEvent(entId, &ETUIBox::ET_alignInBox, childBox);
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
        listType = ListType::Vertical;
    } else if(list == "horz") {
        listType = ListType::Horizontal;
    } else {
        LogWarning("[UIList::serialize] Invalid list type: %s", list);
        return false;
    }
    for(auto& childNode : childrenNode) {
        std::string childObjName;
        childNode.value(childObjName);
        EntityId childEntId;
        ET_SendEventReturn(childEntId, &ETGame::ET_createGameObject, childObjName);
        if(childEntId != InvalidEntityId) {
            children.push_back(childEntId);
        }
    }
    return true;
}

bool UIList::init() {
    if(!UIBox::init()) {
        LogWarning("[UIList::init] UIBox init failed");
        return false;
    }
    std::vector<EntityId> elems = std::move(children);
    for(auto entId : elems) {
        ET_addElement(entId);
    }
    ETNode<ETUIList>::connect(getEntityId());
    return true;
}