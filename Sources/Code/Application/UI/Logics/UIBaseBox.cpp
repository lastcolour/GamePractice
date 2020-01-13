#include "UI/Logics/UIBaseBox.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include <cassert>
#include <algorithm>

UIBaseBox::UIBaseBox() :
    style(),
    box(0),
    lastResizeBox(0),
    isVisible(true) {
}

UIBaseBox::~UIBaseBox() {
}

void UIBaseBox::ET_onChildAdded(EntityId childId) {
    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    ET_SendEvent(childId, &ETEntity::ET_setTransform, tm);
    ET_SendEvent(childId, &ETUIBox::ET_boxResize);
}

void UIBaseBox::ET_alignInBox(const AABB2Di& alignBox) {
    Vec2i center = calcCenter(box, alignBox);
    box.setCenter(center);
    syncTransform();
}

void UIBaseBox::ET_setCenter(const Vec2i& center) {
    box.setCenter(center);
    syncTransform();
}

const AABB2Di& UIBaseBox::ET_getAabb2di() const {
    return box;
}

AABB2Di UIBaseBox::getParentAabb2di() const {
    AABB2Di parentAabb;
    parentAabb.bot = Vec2i(0);
    ET_SendEventReturn(parentAabb.top, &ETRenderCamera::ET_getRenderPort);
    auto parentId = getParentId();
    if(parentId != InvalidEntityId) {
        ET_SendEventReturn(parentAabb, parentId, &ETUIBox::ET_getAabb2di);
    }
    return parentAabb;
}

void UIBaseBox::ET_onTransformChanged(const Transform& newTm) {
    Vec2i newCenter = Vec2i(static_cast<int>(newTm.pt.x), static_cast<int>(newTm.pt.y));
    if(box.getCenter() != newCenter) {
        box.setCenter(newCenter);
    }
}

const Margin& UIBaseBox::ET_getMaring() const {
    return margin;
}

Margin UIBaseBox::calculateMargin(const AABB2Di& parentBox) const {
    Margin resMargin;
    switch(style.sizeInv)
    {
        case SizeInvariant::Absolute:
        {
            Vec2i renderPort(0);
            ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

            resMargin.left = static_cast<int>(renderPort.x * style.margin.left);
            resMargin.right = static_cast<int>(renderPort.x * style.margin.right);
            resMargin.bot = static_cast<int>(renderPort.y * style.margin.bot);
            resMargin.top = static_cast<int>(renderPort.y * style.margin.top);
            break;
        }
        case SizeInvariant::AbsoluteBiggestSquare:
        {
            Vec2i renderPort(0);
            ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);
            int minSide = std::min(renderPort.x, renderPort.y);

            resMargin.left = static_cast<int>(minSide * style.margin.left);
            resMargin.right = static_cast<int>(minSide * style.margin.right);
            resMargin.bot = static_cast<int>(minSide * style.margin.bot);
            resMargin.top = static_cast<int>(minSide * style.margin.top);
            break;
        }
        case SizeInvariant::Relative:
        {
            Vec2i parentSize = parentBox.getSize();

            resMargin.left = static_cast<int>(parentSize.x * style.margin.left);
            resMargin.right = static_cast<int>(parentSize.x * style.margin.right);
            resMargin.bot = static_cast<int>(parentSize.y * style.margin.bot);
            resMargin.top = static_cast<int>(parentSize.y * style.margin.top);
            break;
        }
        case SizeInvariant::RelativeBiggestSquare:
        {
            Vec2i parentSize = parentBox.getSize();
            int minSide = std::min(parentSize.x, parentSize.y);

            resMargin.left = static_cast<int>(minSide * style.margin.left);
            resMargin.right = static_cast<int>(minSide * style.margin.right);
            resMargin.bot = static_cast<int>(minSide * style.margin.bot);
            resMargin.top = static_cast<int>(minSide * style.margin.top);
            break;
        }
        case SizeInvariant::Pixel:
        {
            resMargin.left = static_cast<int>(style.margin.left);
            resMargin.right = static_cast<int>(style.margin.right);
            resMargin.bot = static_cast<int>(style.margin.bot);
            resMargin.top = static_cast<int>(style.margin.top);
            break;
        }
        default:
            assert(false && "Invalid style size invariant type");
            break;
    }
    return resMargin;
}

Vec2i UIBaseBox::calculateBoxSize(const AABB2Di& parentBox) const {
    Vec2i resSize(0);
    switch(style.sizeInv)
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

Vec2i UIBaseBox::calcCenter(const AABB2Di& selfBox, const AABB2Di& parentBox) const {
    Vec2i resCenter = selfBox.getCenter();
    const auto halfSize = selfBox.getSize() / 2;
    switch(style.xAlignType) {
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
    switch(style.yAlignType) {
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

void UIBaseBox::setBox(const AABB2Di& newBox) {
    box = newBox;
    margin = calculateMargin(getParentAabb2di());
    syncTransform();
}

void UIBaseBox::syncTransform() const {
    Transform tm;
    auto center = box.getCenter();
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    tm.pt = Vec3(static_cast<float>(center.x), static_cast<float>(center.y), 0.f);
    ET_SendEvent(getEntityId(), &ETEntity::ET_setTransform, tm);
}

bool UIBaseBox::isNeedResize() {
    auto currParentBox = getParentAabb2di();
    if(currParentBox == lastResizeBox) {
        return false;
    }
    lastResizeBox = currParentBox;
    return true;
}

void UIBaseBox::ET_boxResize() {
    if(!isNeedResize()) {
        return;
    }
    lastResizeBox = getParentAabb2di();
    setBox(calcBox(lastResizeBox));
    std::vector<EntityId> childrenIds;
    ET_SendEventReturn(childrenIds, getEntityId(), &ETEntity::ET_getChildren);
    for(auto childId : childrenIds) {
        ET_SendEvent(childId, &ETUIBox::ET_boxResize);
    }
    ET_SendEvent(getEntityId(), &ETUIBoxEvents::ET_onBoxResized);
}

void UIBaseBox::ET_onRenderPortResized() {
    auto uiList = getRootUIList();
    if(uiList == InvalidEntityId) {
        ET_boxResize();
    }
}

AABB2Di UIBaseBox::calcBox(const AABB2Di& parentBox) const {
    AABB2Di resAabb;
    resAabb.bot = Vec2i(0);
    resAabb.top = calculateBoxSize(parentBox);
    resAabb.setCenter(calcCenter(resAabb, parentBox));
    return resAabb;
}

bool UIBaseBox::serialize(const JSONNode& node) {
    auto styleNode = node.object("style");
    if(!styleNode) {
        LogWarning("[UIBaseBox::init] Can't find require style node");
        return false;
    }
    style.serialize(styleNode);
    return true;
}

const UIStyle& UIBaseBox::ET_getStyle() const {
    return style;
}

void UIBaseBox::ET_setStyle(const UIStyle& newStyle) {
    style = newStyle;
    forceResizeFromTop();
}

bool UIBaseBox::init() {
    lastResizeBox = getParentAabb2di();
    setBox(calcBox(lastResizeBox));
    ETNode<ETUIBox>::connect(getEntityId());
    ETNode<ETRenderEvents>::connect(getEntityId());
    ETNode<ETEntityEvents>::connect(getEntityId());
    return true;
}

void UIBaseBox::forceResizeFromTop() {
    lastResizeBox = AABB2Di(0);
    auto rootEntId = getRootUIList();
    if(rootEntId == InvalidEntityId) {
        ET_boxResize();
    } else {
        ET_SendEvent(rootEntId, &ETUIBox::ET_boxResize);
    }
}

EntityId UIBaseBox::getRootUIList() const {
    auto entId = getParentId();
    auto lastValidEntId = entId;
    while(entId != InvalidEntityId) {
        lastValidEntId = entId;
        ET_SendEventReturn(entId, entId, &ETEntity::ET_getParentId);
        if(!ET_IsExistNode<ETUIList>(entId)) {
            break;
        }
    }
    return lastValidEntId;
}

void UIBaseBox::ET_show() {
    isVisible = true;
    std::vector<EntityId> children;
    ET_SendEventReturn(children, getEntityId(), &ETEntity::ET_getChildren);
    for(const auto& childEntId : children) {
        ET_SendEvent(childEntId, &ETUIBox::ET_show);
    }
}

void UIBaseBox::ET_hide() {
    isVisible = false;
    std::vector<EntityId> children;
    ET_SendEventReturn(children, getEntityId(), &ETEntity::ET_getChildren);
    for(const auto& childEntId : children) {
        ET_SendEvent(childEntId, &ETUIBox::ET_hide);
    }
}

bool UIBaseBox::ET_isVisible() const {
    return isVisible;
}

void UIBaseBox::ET_disableInteraction() {
    std::vector<EntityId> children;
    ET_SendEventReturn(children, getEntityId(), &ETEntity::ET_getChildren);
    for(const auto& childEntId : children) {
        ET_SendEvent(childEntId, &ETUIBox::ET_disableInteraction);
    }
}

void UIBaseBox::ET_enableInteraction() {
    std::vector<EntityId> children;
    ET_SendEventReturn(children, getEntityId(), &ETEntity::ET_getChildren);
    for(const auto& childEntId : children) {
        ET_SendEvent(childEntId, &ETUIBox::ET_enableInteraction);
    }
}