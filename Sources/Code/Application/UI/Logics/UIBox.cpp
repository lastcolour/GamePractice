#include "UI/Logics/UIBox.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Game/GameETInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include <cassert>
#include <algorithm>

UIBox::UIBox() {
}

UIBox::~UIBox() {
}

void UIBox::ET_alignInBox(const AABB2Di& alignBox) {
    Vec2i center = calcCenter(box, alignBox);
    box.setCenter(center);
    syncTransform();
}

void UIBox::ET_setCenter(const Vec2i& center) {
    box.setCenter(center);
    syncTransform();
}

const AABB2Di& UIBox::ET_getAabb2di() const {
    return box;
}

AABB2Di UIBox::ET_getParentAaabb2di() const {
    AABB2Di parentAabb;
    parentAabb.bot = Vec2i(0);
    ET_SendEventReturn(parentAabb.top, &ETRender::ET_getRenderPort);
    if(getParentId() != InvalidEntityId) {
        ET_SendEventReturn(parentAabb, getParentId(), &ETUIBox::ET_getAabb2di);
    }
    return parentAabb;
}

Vec2i UIBox::calcSize(const AABB2Di& parentBox) const {
    Vec2i resSize(0);
    switch (style.sizeInv)
    {
        case SizeInvariant::Absolute:
        {
            Vec2i renderPort;
            ET_SendEventReturn(renderPort, &ETRender::ET_getRenderPort);
            resSize.x = static_cast<int>(renderPort.x * style.size.x);
            resSize.y = static_cast<int>(renderPort.y * style.size.y);
            break;
        }
        case SizeInvariant::AbsoluteBiggestSquare:
        {
            Vec2i renderPort;
            ET_SendEventReturn(renderPort, &ETRender::ET_getRenderPort);
            int minSide = std::min(renderPort.x, renderPort.y);
            resSize.x = static_cast<int>(minSide * style.size.x);
            resSize.y = static_cast<int>(minSide * style.size.y);
            break;
        }
        case SizeInvariant::Relative:
        {
            Vec2i parentSize = parentBox.getSize();
            resSize.x = static_cast<int>(parentSize.x * style.size.x);
            resSize.y = static_cast<int>(parentSize.y * style.size.y);
            break;
        }
        case SizeInvariant::RelativeBiggestSquare:
        {
            Vec2i parentSize = parentBox.getSize();
            int minSide = std::min(parentSize.x, parentSize.y);
            resSize.x = static_cast<int>(minSide * style.size.x);
            resSize.y = static_cast<int>(minSide * style.size.y);
            break;
        }
        case SizeInvariant::Pixel:
        {
            resSize.x = static_cast<int>(style.size.x);
            resSize.y = static_cast<int>(style.size.y);
            break;
        }
        default:
            assert(false && "Invalid style size invariant type");
            break;
    }
    return resSize;
}

Vec2i UIBox::calcCenter(const AABB2Di& selfBox, const AABB2Di& parentBox) const {
    Vec2i resCenter = selfBox.getCenter();
    const auto halfSize = selfBox.getSize() / 2;
    switch (style.xAlignType) {
        case XAlignType::Center: {
            resCenter.x = parentBox.getCenter().x;
            break;
        }
        case XAlignType::Left: {
            resCenter.x = parentBox.bot.x + halfSize.x;
            break;
        }
        case XAlignType::Right: {
            resCenter.x = parentBox.top.x - halfSize.x;
            break;
        }
        default:
            assert(false && "Invalid align type");
    }
    switch (style.yAlignType) {
        case YAlignType::Center: {
            resCenter.y = parentBox.getCenter().y;
            break;
        }
        case YAlignType::Top: {
            resCenter.y = parentBox.top.y - halfSize.y;
            break;
        }
        case YAlignType::Bot: {
            resCenter.y = parentBox.bot.y + halfSize.y;
            break;
        }
        default:
            assert(false && "Invalid align type");
    }
    return resCenter;
}

void UIBox::setBox(const AABB2Di& newBox) {
    box = newBox;
    syncTransform();
}

void UIBox::syncTransform() const {
    Transform tm;
    auto center = box.getCenter();
    ET_SendEventReturn(tm, getEntityId(), &ETGameObject::ET_getTransform);
    tm.pt = Vec3(static_cast<float>(center.x), static_cast<float>(center.y), 0.f);
    ET_SendEvent(getEntityId(), &ETGameObject::ET_setTransform, tm);

    const Vec2i size = box.getSize();
    RenderLogicParams params;
    params.col = style.color;
    params.size = Vec2(static_cast<float>(size.x), static_cast<float>(size.y));
    ET_SendEvent(getEntityId(), &ETRenderLogic::ET_setRenderParams, params);
}

void UIBox::ET_boxResize() {
    setBox(calcBox());
}

void UIBox::ET_onRenderPortResized() {
    if(!ET_IsExistNode<ETUIList>(getParentId())) {
        ET_boxResize();
    }
}

const UIStyle& UIBox::getStyle() const {
    return style;
}

void UIBox::setStyle(const UIStyle& uiStyle) {
    style = uiStyle;
}

AABB2Di UIBox::calcBox() const {
    auto parentBox = ET_getParentAaabb2di();
    AABB2Di resAabb;
    resAabb.bot = Vec2i(0);
    resAabb.top = calcSize(parentBox);
    resAabb.setCenter(calcCenter(resAabb, parentBox));
    return resAabb;
}

bool UIBox::serialize(const JSONNode& node) {
    auto styleNode = node.object("style");
    if(!styleNode) {
        LogWarning("[UIList::init] Can't find require style node");
        return false;
    }
    if(!style.serialize(styleNode)) {
        LogWarning("[UIBox::serialize] Can't serialize style node");
        return false;
    }
    return true;
}

bool UIBox::init() {
    setBox(calcBox());
    ETNode<ETUIBox>::connect(getEntityId());
    ETNode<ETRenderEvents>::connect(getEntityId());
    return true;
}