#include "UI/Logics/UIBox.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include <cassert>

UIBox::UIBox() {
}

UIBox::~UIBox() {
}

const AABB2Di& UIBox::ET_getAaabb2di() const {
    return box;
}

AABB2Di UIBox::caclParentBox() const {
    AABB2Di parentAabb;
    parentAabb.bot = Vec2i(0);
    ET_SendEventReturn(parentAabb.top, &ETRender::ET_getRenderPort);
    if(getParentId() != InvalidEntityId) {
        ET_SendEventReturn(parentAabb, getParentId(), &ETUIBox::ET_getAaabb2di);
    }
    return parentAabb;
}

Vec2i UIBox::calcSize(const AABB2Di& parentBox) const {
    Vec2i parentSize = parentBox.getSize();
    Vec2i resSize = parentSize;
    resSize.x = static_cast<int>(resSize.x * style.size.x);
    resSize.y = static_cast<int>(resSize.y * style.size.y);
    return resSize;
}

Vec2i UIBox::calcCenter(const AABB2Di& parentBox) const {
    auto parentCenter = parentBox.getCenter();
    Vec2i resCenter(0);
    if(style.listPos != InvalidListPos) {
        Vec2i offset;
        ET_SendEventReturn(offset, getParentId(), &ETUIList::ET_getElemOffset, style.listPos);
        ListType listType;
        ET_SendEventReturn(listType, getParentId(), &ETUIList::ET_getListType);
        if(listType == ListType::Horizontal) {
            resCenter = Vec2i(parentCenter.x, offset.y);
        } else {
            resCenter = Vec2i(offset.x, parentCenter.y);
        }
    }
    switch (style.alignType)
    {
        case AlignType::Center:
        {
            resCenter = parentCenter;
            break;
        }
        case AlignType::Left:
        {
            assert(false && "Not implemented");
            break;
        }
        case AlignType::Right:
        {
            assert(false && "Not implemented");
            break;
        }
        default:
            assert(false && "Invalid align type");
    }
    return resCenter;
}

AABB2Di UIBox::calcBox() const {
    auto parentBox = caclParentBox();
    AABB2Di resAabb;
    resAabb.bot = Vec2i(0);
    resAabb.top = calcSize(parentBox);
    resAabb.setCenter(calcCenter(parentBox));
    return resAabb;
}

void UIBox::ET_setUIListPos(int pos) {
    style.listPos = pos;
    calcBox();
}

bool UIBox::serialize(const JSONNode& node) {
    auto styleNode = node.object("style");
    if(!styleNode) {
        LogWarning("[UIList::init] Can't find require style node");
        return false;
    }
    style.serialize(styleNode);
    return true;
}

bool UIBox::init() {
    box = calcBox();
    ETNode<ETUIBox>::connect(getEntityId());
    return true;
}
