#include "UI/Logics/UIList.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include <cassert>

UIList::UIList() {
}

UIList::~UIList() {
}

Vec2i UIList::ET_getElemOffset(int idx) const {
    assert(idx >= 0 && "Negative list pos");
    if(idx == 0) {
        return Vec2i(0);
    }
    assert(idx <= children.size() && "Out of bound list pos");
    auto childId = children[idx - 1];
    AABB2Di childBox;
    AABB2Di selfBox = ET_getAaabb2di();
    ET_SendEventReturn(childBox, childId, &ETUIBox::ET_getAaabb2di);
    if(style.listType == ListType::Horizontal) {
        Vec2i offset = selfBox.top - childBox.bot;
        return offset;
    } else {
        return Vec2i(0);
    }
}

ListType UIList::ET_getListType() const {
    return style.listType;
}

void UIList::addElement(EntityId entId) {
    ET_SendEvent(getEntityId(), &ETGameObject::ET_addChild, entId);
    ET_SendEvent(entId, &ETUIBox::ET_setUIListPos, children.size());
    children.push_back(entId);
}

void UIList::calcResListBox() {
    if(children.empty()) {
        box = AABB2Di(0);
        return;
    }
    AABB2Di childBox;
    ET_SendEventReturn(childBox, children[0], &ETUIBox::ET_getAaabb2di);
    box = childBox;
    for(size_t i = 1, sz = children.size(); i < sz; ++i) {
        ET_SendEventReturn(box, children[0], &ETUIBox::ET_getAaabb2di);
        if(style.listType == ListType::Horizontal) {
            box.top.y = std::max(box.top.y, childBox.top.y);
            box.bot.y = childBox.bot.y;
            box.bot.x = std::min(box.top.x, childBox.bot.x);
        } else {
        }
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
    for(auto& childNode : childrenNode) {
        std::string childObjName;
        childNode.value(childObjName);
        EntityId childEntId;
        ET_SendEventReturn(childEntId, &ETGame::ET_createGameObject, childObjName);
        if(childEntId != InvalidEntityId) {
            children.push_back(childEntId);
        }
    }
}

bool UIList::init() {
    if(!UIBox::init()) {
        LogWarning("[UIList::init] UIBox init failed");
        return false;
    }

    ETNode<ETUIList>::connect(getEntityId());

    std::vector<EntityId> elem = std::move(children);
    for(auto entId : elem) {
        addElement(entId);
    }
    calcResListBox();
    return true;
}