#include "UI/Logics/UIBox.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include <cassert>
#include <algorithm>

UIBox::UIBox() {
}

UIBox::~UIBox() {
}

void UIBox::ET_onChildAdded(EntityId childId) {
    ET_SendEvent(childId, &ETUIBox::ET_boxResize);
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

EntityId UIBox::getRendererId() const {
    return renderId;
}

const AABB2Di& UIBox::ET_getAabb2di() const {
    return box;
}

AABB2Di UIBox::getParentAabb2di() const {
    AABB2Di parentAabb;
    parentAabb.bot = Vec2i(0);
    ET_SendEventReturn(parentAabb.top, &ETRenderCamera::ET_getRenderPort);
    if(getParentId() != InvalidEntityId) {
        ET_SendEventReturn(parentAabb, getParentId(), &ETUIBox::ET_getAabb2di);
    }
    return parentAabb;
}

void UIBox::ET_onTransformChanged(const Transform& newTm) {
    Vec2i newCenter = Vec2i(static_cast<int>(newTm.pt.x), static_cast<int>(newTm.pt.y));
    if(box.getCenter() != newCenter) {
        box.setCenter(newCenter);
    }
}

Vec2i UIBox::calculateBoxSize(const AABB2Di& parentBox) const {
    Vec2i resSize(0);
    switch (style.sizeInv)
    {
        case SizeInvariant::Absolute:
        {
            Vec2i renderPort(0);
            ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);
            resSize.x = static_cast<int>(renderPort.x * style.size.x);
            resSize.y = static_cast<int>(renderPort.y * style.size.y);
            break;
        }
        case SizeInvariant::AbsoluteBiggestSquare:
        {
            Vec2i renderPort(0);
            ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);
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
    const Vec2i size = box.getSize();
    ET_SendEvent(renderId, &ETRenderSimpleLogic::ET_setSize, Vec2(static_cast<float>(size.x), static_cast<float>(size.y)));

    syncTransform();
}

void UIBox::syncTransform() const {
    Transform tm;
    auto center = box.getCenter();
    ET_SendEventReturn(tm, getEntityId(), &ETGameObject::ET_getTransform);
    tm.pt = Vec3(static_cast<float>(center.x), static_cast<float>(center.y), 0.f);
    ET_SendEvent(getEntityId(), &ETGameObject::ET_setTransform, tm);
}

void UIBox::ET_boxResize() {
    setBox(calcBox(getParentAabb2di()));
    std::vector<EntityId> childrenIds;
    ET_SendEventReturn(childrenIds, getEntityId(), &ETGameObject::ET_getChildren);
    for(auto childId : childrenIds) {
        ET_SendEvent(childId, &ETUIBox::ET_boxResize);
    }
    ET_SendEvent(getEntityId(), &ETUIBoxEvents::ET_onBoxResized);
}

void UIBox::ET_onRenderPortResized() {
    if(!ET_IsExistNode<ETUIList>(getParentId())) {
        ET_boxResize();
    }
}

AABB2Di UIBox::calcBox(const AABB2Di& parentBox) const {
    AABB2Di resAabb;
    resAabb.bot = Vec2i(0);
    resAabb.top = calculateBoxSize(parentBox);
    resAabb.setCenter(calcCenter(resAabb, parentBox));
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

bool UIBox::createRenderer() {
    if(renderId.isValid()) {
        ET_SendEvent(&ETGameObjectManager::ET_destroyObject, renderId);
        renderId = InvalidEntityId;
    }
    if(!style.renderer.empty()) {
        ET_SendEventReturn(renderId, &ETGameObjectManager::ET_createGameObject, style.renderer.c_str());
        if(renderId.isValid()) {
            ET_SendEvent(renderId, &ETGameObject::ET_setParent, getEntityId());
            ET_SendEvent(renderId, &ETRenderSimpleLogic::ET_setColor, style.color);
        } else {
            LogWarning("[UIBox::init] Can't create renderer: %s", style.renderer);
            return false;
        }
    }
    return true;
}

const UIStyle& UIBox::ET_getStyle() const {
    return style;
}

void UIBox::ET_setStyle(const UIStyle& newStyle) {
    bool updateRender = false;
    if(style.renderer != newStyle.renderer) {
        updateRender = true;
    }
    style = newStyle;
    if (updateRender) {
        createRenderer();
    }
    updateRendererParams();
    auto rootEntId = getRootUIBox();
    if(rootEntId == InvalidEntityId) {
        ET_boxResize();
    } else {
        ET_SendEvent(rootEntId, &ETUIBox::ET_boxResize);
    }
}

void UIBox::updateRendererParams() {
    if(!renderId.isValid()) {
        return;
    }
    if(ET_IsExistNode<ETRenderSimpleLogic>(renderId)) {
        ET_SendEvent(renderId, &ETRenderSimpleLogic::ET_setColor, style.color);
    } else if(ET_IsExistNode<ETRenderTextLogic>(renderId)) {
        ET_SendEvent(getRendererId(), &ETRenderTextLogic::ET_setColor, style.color);
    }
}

bool UIBox::init() {
    createRenderer();
    updateRendererParams();
    setBox(calcBox(getParentAabb2di()));
    ETNode<ETUIBox>::connect(getEntityId());
    ETNode<ETRenderEvents>::connect(getEntityId());
    ETNode<ETGameObjectEvents>::connect(getEntityId());
    return true;
}

EntityId UIBox::getRootUIBox() const {
    auto entId = getParentId();
    auto lastValidEntId = entId;
    while(entId != InvalidEntityId) {
        lastValidEntId = entId;
        ET_SendEventReturn(entId, entId, &ETGameObject::ET_getParentId);
        if (!ET_IsExistNode<ETUIBox>(entId)) {
            break;
        }
    }
    return lastValidEntId;
}
