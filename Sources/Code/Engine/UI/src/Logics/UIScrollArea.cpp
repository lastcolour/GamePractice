#include "UIScrollArea.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntity.hpp"
#include "UIUtils.hpp"

#include <algorithm>
#include <cassert>

namespace {

template<typename T>
T getMaxByMagnitude(T val, T testVal) {
    if(testVal <= static_cast<T>(0)) {
        return std::min(val, testVal);
    }
    return std::max(val, testVal);
}

template<typename T>
T getMinByMagnitude(T val, T testVal) {
    if(testVal <= static_cast<T>(0)) {
        return std::max(val, testVal);
    }
    return std::min(val, testVal);
}

} // namespace

void UIScrollArea::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIScrollArea>("UIScrollArea")) {
        classInfo->addField("style", &UIScrollArea::style);
        classInfo->addField("target", &UIScrollArea::targetId);
    }
}

UIScrollArea::UIScrollArea() :
    endScrollPt(0),
    scrollSpeed(0.f),
    accumulativeDt(0.f),
    minPt(0),
    maxPt(0),
    isPressed(false) {
}

UIScrollArea::~UIScrollArea() {
}

bool UIScrollArea::init() {
    ETNode<ETUIScrollArea>::connect(getEntityId());
    ETNode<ETUIInteractionBox>::connect(getEntityId());
    ETNode<ETUIElementEvents>::connect(getEntityId());
    ETNode<ETUIElemAligner>::connect(getEntityId());
    initScrollElem();
    return true;
}

void UIScrollArea::initScrollElem() {
    if(!targetId.isValid()) {
        return;
    }
    if(targetId == getEntityId()) {
        return;
    }

    AABB2Di elemBox(Vec2i(0), Vec2i(0));
    ET_SendEventReturn(elemBox, targetId, &ETUIElement::ET_getBox);

    elemBox.top = elemBox.getSize();
    elemBox.bot = Vec2i(0);

    AABB2Di box = ET_getHitBox();
    auto center = box.getCenter();

    box.top = box.getSize();
    box.bot = Vec2i(0);

    if(style.type == UIScrollType::Horizontal) {
        minPt = UI::CalcAligmentCenter(UIXAlign::Left, UIYAlign::Center, box, elemBox);
        maxPt = UI::CalcAligmentCenter(UIXAlign::Right, UIYAlign::Center, box, elemBox);
    } else {
        minPt = UI::CalcAligmentCenter(UIXAlign::Center, UIYAlign::Top, box, elemBox);
        maxPt = UI::CalcAligmentCenter(UIXAlign::Center, UIYAlign::Bot, box, elemBox);
    }

    auto size = box.getSize();

    minPt = center + minPt - size / 2;
    maxPt = center + maxPt - size / 2;

    if(style.origin == UIScrollOrigin::Start) {
        if(style.type == UIScrollType::Vertical) {
            UI::Set2DPositionDoNotUpdateLayout(targetId, maxPt);
        } else {
            UI::Set2DPositionDoNotUpdateLayout(targetId, minPt);
        }
    } else {
        if(style.type == UIScrollType::Vertical) {
            UI::Set2DPositionDoNotUpdateLayout(targetId, minPt);
        } else {
            UI::Set2DPositionDoNotUpdateLayout(targetId, maxPt);
        }
    }

    assert(minPt <= maxPt && "Invalid min-max pos for scroll area");

    auto childZIndex = UI::GetZIndexForChild(getEntityId());
    ET_SendEvent(targetId, &ETUIElement::ET_setZIndex, childZIndex);
}

void UIScrollArea::deinit() {
}

EInputEventResult UIScrollArea::ET_onInputEvent(EActionType type, const Vec2i& pt) {
    switch(type) {
        case EActionType::Press: {
            onPress(pt);
            break;
        }
        case EActionType::Move: {
            if(!onMove(pt)) {
                return EInputEventResult::Ignore;
            }
            break;
        }
        case EActionType::Release: {
            onRelease(pt);
            break;
        }
    }
    return EInputEventResult::Accept;
}

AABB2Di UIScrollArea::ET_getHitBox() const {
    AABB2Di box;
    ET_SendEventReturn(box, getEntityId(), &ETUIElement::ET_getBox);
    return box;
}

void UIScrollArea::onPress(const Vec2i& pt) {
    accumulativeDt = 0.f;
    isPressed = true;
    ETNode<ETUITimerEvents>::connect(getEntityId());
    path.clear();
    path.push_back({TimePoint::GetNowTime(), pt});
}

void UIScrollArea::onRelease(const Vec2i& pt) {
    isPressed = false;
    path.push_back({TimePoint::GetNowTime(), pt});
}

bool UIScrollArea::onMove(const Vec2i& pt) {
    auto box = ET_getHitBox();
    if(!UI::IsInsideBox(pt, box)) {
        onRelease(pt);
        return false;
    } else {
        path.push_back({TimePoint::GetNowTime(), pt});
    }
    return true;
}

void UIScrollArea::ET_onBoxChanged(const AABB2Di& newAabb) {
    initScrollElem();
}

void UIScrollArea::ET_onZIndexChanged(int newZIndex) {
    auto childZIndex = UI::GetZIndexForChild(getEntityId());
    ET_SendEvent(targetId, &ETUIElement::ET_setZIndex, childZIndex);
}

void UIScrollArea::ET_onAlphaChanged(float newAlpha) {
    ET_SendEvent(targetId, &ETUIElement::ET_setAlpha, newAlpha);
}

void UIScrollArea::ET_onHidden(bool flag) {
    if(flag) {
        ET_SendEvent(targetId, &ETUIElement::ET_hide);
    } else {
        ET_SendEvent(targetId, &ETUIElement::ET_show);
    }
}

void UIScrollArea::ET_onDisabled(bool flag) {
    if(flag) {
        ET_SendEvent(targetId, &ETUIElement::ET_disable);
    } else {
        ET_SendEvent(targetId, &ETUIElement::ET_enable);
    }
}

void UIScrollArea::ET_onIngoreTransform(bool flag) {
    ET_SendEvent(targetId, &ETUIElement::ET_setIgnoreTransform, flag);
}

void UIScrollArea::ET_onUITick(float dt) {
    accumulativeDt += dt;

    Transform tm;
    ET_SendEventReturn(tm, targetId, &ETEntity::ET_getTransform);
    Vec2i targetPt(static_cast<int>(tm.pt.x), static_cast<int>(tm.pt.y));

    Vec2i ptShift(0);
    float shiftTime = 0.f;

    if(path.size() > 1) {
        auto& startEvent = path.front();
        auto endEvent = path.back();

        ptShift = endEvent.pt - startEvent.pt;
        shiftTime = endEvent.timeP.getSecondsElapsedFrom(startEvent.timeP);
        shiftTime = std::max(shiftTime, 0.016f);

        endScrollPt = targetPt + ptShift;
        endScrollPt.x = Math::Clamp(endScrollPt.x, minPt.x, maxPt.x);
        endScrollPt.y = Math::Clamp(endScrollPt.y, maxPt.y, minPt.y);

        path.clear();
        path.push_back(endEvent);
    }

    Vec2i deltaShift(0);

    if(style.type == UIScrollType::Horizontal) {
        auto newSpeed = ptShift.x / shiftTime;
        scrollSpeed = getMaxByMagnitude(scrollSpeed, newSpeed);
        deltaShift.x = static_cast<int>(scrollSpeed * accumulativeDt);
        if(std::abs(deltaShift.x) > 0) {
            accumulativeDt -= deltaShift.x / scrollSpeed;
        }
    } else {
        auto newSpeed = ptShift.y / shiftTime;
        scrollSpeed = getMaxByMagnitude(scrollSpeed, newSpeed);
        deltaShift.y = static_cast<int>(scrollSpeed * accumulativeDt);
        if(std::abs(deltaShift.y) > 0) {
            accumulativeDt -= deltaShift.y / scrollSpeed;
        }
    }

    auto remainingShift = endScrollPt - targetPt;
    deltaShift.x = getMinByMagnitude(deltaShift.x, remainingShift.x);
    deltaShift.y = getMinByMagnitude(deltaShift.y, remainingShift.y);

    targetPt += deltaShift;
    targetPt.x = Math::Clamp(targetPt.x, minPt.x, maxPt.x);
    targetPt.y = Math::Clamp(targetPt.y, maxPt.y, minPt.y);

    UI::Set2DPositionDoNotUpdateLayout(targetId, targetPt);

    if(targetPt == endScrollPt && !isPressed) {
        ETNode<ETUITimerEvents>::disconnect();
    }
}

void UIScrollArea::ET_setTarget(EntityId newTargetId) {
    targetId = newTargetId;
    initScrollElem();
}

EntityId UIScrollArea::ET_getTarget() const {
    return targetId;
}

void UIScrollArea::ET_setStyle(const UIScrollAreaStyle& newStyle) {
    style = newStyle;
    initScrollElem();
}

const UIScrollAreaStyle& UIScrollArea::ET_getStyle() const {
    return style;
}

void UIScrollArea::ET_reAlign() {
    initScrollElem();
}