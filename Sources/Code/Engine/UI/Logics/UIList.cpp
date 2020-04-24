#include "UI/Logics/UIList.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include <cassert>
#include <algorithm>
#include <limits>

struct OffsetData {
    int offset;
    int margin;
};

UIList::UIList() :
    listType(UIListType::Vertical) {
}

UIList::~UIList() {
}

bool UIList::serialize(const JSONNode& node) {
    if(!UIBaseBox::serialize(node)) {
        LogWarning("[UIList::serialize] UIBaseBox searilization failed");
        return false;
    }
    std::string list("vert");
    node.read("type", list);
    if(list == "vert") {
        listType = UIListType::Vertical;
    } else if(list == "horz") {
        listType = UIListType::Horizontal;
    } else {
        LogWarning("[UIList::serialize] Invalid list type: %s", list);
        return false;
    }
    return true;
}

bool UIList::init() {
    if(!UIBaseBox::init()) {
        LogWarning("[UIList::init] UIBaseBox init failed");
        return false;
    }
    ETNode<ETUIList>::connect(getEntityId());
    calcList();
    return true;
}

void UIList::ET_onChildAdded(EntityId newElemId) {
    children.push_back(newElemId);
    calcList();
}

AABB2Di UIList::getAligntBox(const AABB2Di& elemBox) const {
    AABB2Di listBox = ET_getAabb2di();
    AABB2Di resBox(0);
    Vec2i elemBoxSize = elemBox.getSize();
    if(listType == UIListType::Vertical) {
        resBox.bot = Vec2i(listBox.bot.x, 0);
        resBox.top = Vec2i(listBox.top.x, elemBoxSize.y);
    } else {
        resBox.bot = Vec2i(0, listBox.bot.y);
        resBox.top = Vec2i(elemBoxSize.x, listBox.top.y);
    }
    return resBox;
}

void UIList::initOffset(OffsetData& offsetData) const {
    offsetData.margin = 0;
    if(listType == UIListType::Vertical) {
        offsetData.offset = ET_getAabb2di().top.y;
    } else {
        offsetData.offset = ET_getAabb2di().bot.x;
    }
}

Vec2i UIList::caclCenterUpdateOffset(OffsetData& offsetData, const AABB2Di& elemBox, const Margin& elemMargin) const {
    auto boxSize = elemBox.getSize();
    auto center = elemBox.getCenter();
    if(listType == UIListType::Vertical) {
        int marginShift = std::max(offsetData.margin, elemMargin.top);
        center.y = offsetData.offset - boxSize.y / 2 - marginShift;
        offsetData.offset -= boxSize.y + marginShift;
        offsetData.margin = elemMargin.bot;
    } else {
        int marginShift = std::max(offsetData.margin, elemMargin.left);
        center.x = offsetData.offset + boxSize.x / 2 + marginShift;
        offsetData.offset += boxSize.x + marginShift;
        offsetData.margin = elemMargin.right;
    }
    return center;
}

void UIList::calcList() {
    setBox(calcBox(getParentAabb2di()));

    OffsetData offsetData;
    initOffset(offsetData);
    
    AABB2Di listBox = ET_getAabb2di();
    if(!children.empty()) {

        listBox.top = Vec2i(std::numeric_limits<int>::min());
        listBox.bot = Vec2i(std::numeric_limits<int>::max());

        for(auto entId : children) {
            ET_SendEvent(entId, &ETUIBox::ET_boxResize);
            AABB2Di elemBox(0);
            ET_SendEventReturn(elemBox, entId, &ETUIBox::ET_getAabb2di);

            ET_SendEvent(entId, &ETUIBox::ET_alignInBox, getAligntBox(elemBox));

            ET_SendEventReturn(elemBox, entId, &ETUIBox::ET_getAabb2di);
            Margin elemMargin;
            ET_SendEventReturn(elemMargin, entId, &ETUIBox::ET_getMaring);
            auto newCenter = caclCenterUpdateOffset(offsetData, elemBox, elemMargin);

            ET_SendEvent(entId, &ETUIBox::ET_setCenter, newCenter);

            ET_SendEventReturn(elemBox, entId, &ETUIBox::ET_getAabb2di);
            listBox.bot.x = std::min(listBox.bot.x, elemBox.bot.x);
            listBox.bot.y = std::min(listBox.bot.y, elemBox.bot.y);
            listBox.top.y = std::max(listBox.top.y, elemBox.top.y);
            listBox.top.x = std::max(listBox.top.x, elemBox.top.x);
        }
    }

    auto origBoxCenter = ET_getAabb2di().getCenter();
    if(listType == UIListType::Vertical) {
        auto diff = std::max(origBoxCenter.x - listBox.bot.x, listBox.top.x - origBoxCenter.x);
        listBox.bot.x = origBoxCenter.x - diff;
        listBox.top.x = origBoxCenter.x + diff;
    } else {
        auto diff = std::max(origBoxCenter.y - listBox.bot.y, listBox.top.y - origBoxCenter.y);
        listBox.bot.y = origBoxCenter.y - diff;
        listBox.top.y = origBoxCenter.y + diff;
    }

    auto oldListCenter = listBox.getCenter();
    setBox(listBox);
    ET_alignInBox(getParentAabb2di());
    auto centerShift = origBoxCenter - oldListCenter;

    for(auto entId : children) {
        AABB2Di elemBox(0);
        ET_SendEventReturn(elemBox, entId, &ETUIBox::ET_getAabb2di);
        auto elemCenter = elemBox.getCenter() + centerShift;
        ET_SendEvent(entId, &ETUIBox::ET_setCenter, elemCenter);
    }
}

void UIList::ET_setType(UIListType newListType) {
    if(listType != newListType) {
        listType = newListType;
        calcList();
    }
}

void UIList::ET_boxResize() {
    calcList();
}