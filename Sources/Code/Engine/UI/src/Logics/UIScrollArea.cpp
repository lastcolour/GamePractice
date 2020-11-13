#include "UIScrollArea.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntity.hpp"
#include "UIUtils.hpp"
#include "Core/ETLogger.hpp"

#include <algorithm>
#include <cassert>

namespace {

struct MoveResult {
    int deltaShift;
    float remainingDt;
};

Vec2i clampToEdges(const AABB2Di& box, const Vec2i& pt) {
    Vec2i resPt;
    resPt.x = Math::Clamp(pt.x, box.bot.x, box.top.x);
    resPt.y = Math::Clamp(pt.y, box.bot.y, box.top.y);
    return resPt;
}

float getScrollSpeed(float currentSpeed, float newSpeed) {
    if(newSpeed > 0.f) {
        if(currentSpeed > 0.f) {
            currentSpeed = (currentSpeed + newSpeed) / 2.f;
        } else {
            currentSpeed = newSpeed;
        }
    } else {
        if(currentSpeed < 0.f) {
            currentSpeed = (currentSpeed + newSpeed) / 2.f;
        } else {
            currentSpeed = newSpeed;
        }
    }
    return currentSpeed;
}

MoveResult moveScroll(float dt, float newSpeed, float currentSpeed, int reaminingShift) {
    currentSpeed = getScrollSpeed(currentSpeed, newSpeed);
    int deltaShift = static_cast<int>(currentSpeed * dt);
    if(std::abs(deltaShift) > 0) {
        dt -= deltaShift / currentSpeed;
    }
    if(currentSpeed > 0.f) {
        deltaShift = std::min(deltaShift, reaminingShift);
    } else {
        deltaShift = std::max(deltaShift, reaminingShift);
    }
    MoveResult res;
    res.deltaShift = deltaShift;
    res.remainingDt = dt;
    return res;
}

bool doesScrollSupported(UIScrollType scrollType, const AABB2Di& targetBox, const AABB2Di& box) {
    if(scrollType == UIScrollType::Horizontal) {
        if(targetBox.getSize().x <= box.getSize().x) {
            return false;
        }
    } else {
        if(targetBox.getSize().y <= box.getSize().y) {
            return false;
        }
    }
    return true;
}

} // namespace

void UIScrollArea::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIScrollArea>("UIScrollArea")) {
        classInfo->addField("extraZOffset", &UIScrollArea::extraZOffset);
        classInfo->addField("style", &UIScrollArea::style);
        classInfo->addField("target", &UIScrollArea::targetId);
    }
}

UIScrollArea::UIScrollArea() :
    endScrollPt(0),
    extraZOffset(0),
    scrollSpeed(0.f),
    accumulativeDt(0.f),
    scrollBox(Vec2i(0), Vec2i(0)),
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
        LogWarning("[UIScrollArea::initScrollElem] Can't have host element '%s' as a scroll target",
            EntityUtils::GetEntityName(targetId));
        targetId = InvalidEntityId;
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

    auto size = box.getSize();

    Vec2i botPt(0);
    Vec2i topPt(0);
    if(style.type == UIScrollType::Horizontal) {
        botPt = UI::CalcAligmentCenter(UIXAlign::Right, UIYAlign::Center, box, elemBox);
        topPt = UI::CalcAligmentCenter(UIXAlign::Left, UIYAlign::Center, box, elemBox);
    } else {
        botPt = UI::CalcAligmentCenter(UIXAlign::Center, UIYAlign::Top, box, elemBox);
        topPt = UI::CalcAligmentCenter(UIXAlign::Center, UIYAlign::Bot, box, elemBox);
    }

    scrollBox.bot = center + botPt - size / 2;
    scrollBox.top = center + topPt - size / 2;

    if(style.type == UIScrollType::Vertical) {
        if(style.origin == UIScrollOrigin::Start) {
            UI::Set2DPositionDoNotUpdateLayout(targetId, scrollBox.bot);
        } else {
            UI::Set2DPositionDoNotUpdateLayout(targetId, scrollBox.top);
        }
    } else {
        if(style.origin == UIScrollOrigin::Start) {
            UI::Set2DPositionDoNotUpdateLayout(targetId, scrollBox.top);
        } else {
            UI::Set2DPositionDoNotUpdateLayout(targetId, scrollBox.bot);
        }
    }

    if(!doesScrollSupported(style.type, elemBox, box)) {
        if(style.type == UIScrollType::Vertical) {
            if(style.origin == UIScrollOrigin::Start) {
                scrollBox.top = scrollBox.bot;
            } else {
                scrollBox.bot = scrollBox.top;
            }
        } else {
            if(style.origin == UIScrollOrigin::Start) {
                scrollBox.bot = scrollBox.top;
            } else {
                scrollBox.top = scrollBox.bot;
            }
        }
    }

    auto childZIndex = UI::GetZIndexForChild(getEntityId());
    childZIndex += extraZOffset;
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

    Transform tm;
    ET_SendEventReturn(tm, targetId, &ETEntity::ET_getTransform);
    endScrollPt.x = static_cast<int>(tm.pt.x);
    endScrollPt.y = static_cast<int>(tm.pt.y);

    ETNode<ETUITimerEvents>::connect(getEntityId());
    path.clear();
    path.push_back({TimePoint::GetNowTime(), pt});
}

void UIScrollArea::onRelease(const Vec2i& pt) {
    isPressed = false;
    auto box = ET_getHitBox();
    auto clampPt = clampToEdges(box, pt);
    path.push_back({TimePoint::GetNowTime(), clampPt});
}

bool UIScrollArea::onMove(const Vec2i& pt) {
    auto box = ET_getHitBox();
    if(!box.isInside(pt)) {
        auto clampPt = clampToEdges(box, pt);
        onRelease(clampPt);
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
    childZIndex += extraZOffset;
    ET_SendEvent(targetId, &ETUIElement::ET_setZIndex, childZIndex);
}

void UIScrollArea::ET_onAlphaChanged(float newAlpha) {
    ET_SendEvent(targetId, &ETUIElement::ET_setAlpha, newAlpha);
}

void UIScrollArea::ET_onHidden(bool flag) {
    ET_SendEvent(targetId, &ETUIElement::ET_setParentHidden, flag);
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
        shiftTime = endEvent.timeP.getSecElapsedFrom(startEvent.timeP);
        shiftTime = std::max(shiftTime, 0.016f);

        endScrollPt = targetPt + ptShift;
        endScrollPt = clampToEdges(scrollBox, endScrollPt);

        path.clear();
        path.push_back(endEvent);
    }

    auto remainingShift = endScrollPt - targetPt;
    Vec2i deltaShift(0);

    if(style.type == UIScrollType::Horizontal) {
        auto newSpeed = ptShift.x / shiftTime;
        auto state = moveScroll(accumulativeDt, newSpeed, scrollSpeed, remainingShift.x);
        deltaShift.x = state.deltaShift;
        accumulativeDt = state.remainingDt;
    } else {
        auto newSpeed = ptShift.y / shiftTime;
        auto state = moveScroll(accumulativeDt, newSpeed, scrollSpeed, remainingShift.y);
        deltaShift.y = state.deltaShift;
        accumulativeDt = state.remainingDt;
    }

    targetPt += deltaShift;
    targetPt = clampToEdges(scrollBox, targetPt);

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