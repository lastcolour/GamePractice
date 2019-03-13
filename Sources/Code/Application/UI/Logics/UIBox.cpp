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

AABB2Di UIBox::calcBox() const {
    EntityId parentId;
    AABB2Di parentAabb;
    parentAabb.bot = Vec2i(0);
    ET_SendEventReturn(parentAabb.top, &ETRender::ET_getRenderPort);
    ET_SendEventReturn(parentId, getEntityId(), &ETGameObject::ET_getParentId);
    if(parentId != InvalidEntityId) {
        ET_SendEventReturn(parentAabb, parentId, &ETUIBox::ET_getAaabb2di);
    }
    Vec2i parentSize = parentAabb.getSize();
    Vec2i resSize = parentSize;
    resSize.x = static_cast<int>(resSize.x * style.size.x);
    resSize.y = static_cast<int>(resSize.y * style.size.y);

    AABB2Di resAabb;
    resAabb.bot = Vec2i(0);
    resAabb.top = resSize;

    switch (style.alignType)
    {
        case AlignType::Center:
        {
            resAabb.setCenter(parentAabb.getCenter());
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

    return resAabb;
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
