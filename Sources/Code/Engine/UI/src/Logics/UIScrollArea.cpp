#include "UIScrollArea.hpp"
#include "UIUtils.hpp"

#include <cassert>

namespace {

AABB2D calcScrollArea(UIScrollType scrollType, const AABB2D& scrollBox, const AABB2D& targetBox) {
    auto targetSize = targetBox.getSize();
    auto scrollSize = scrollBox.getSize();
    auto scrollCenter = scrollBox.getCenter();

    AABB2D res(0.f);

    if(scrollType == UIScrollType::Horizontal) {
        if(scrollSize.x < targetSize.x) {
            res.bot = UI::CalcAligmentCenter(UIXAlign::Right, UIYAlign::Center, scrollBox, targetBox);
            res.top = UI::CalcAligmentCenter(UIXAlign::Left, UIYAlign::Center, scrollBox, targetBox);
        } else {
            res.bot = scrollCenter;
            res.top = scrollCenter;
        }
    } else {
        if(scrollSize.y < targetSize.y) {
            res.bot = UI::CalcAligmentCenter(UIXAlign::Center, UIYAlign::Top, scrollBox, targetBox);
            res.top = UI::CalcAligmentCenter(UIXAlign::Center, UIYAlign::Bot, scrollBox, targetBox);
        } else {
            res.bot = scrollCenter;
            res.top = scrollCenter;
        }
    }

    return res;
}

AABB2D getUIBox(EntityId targetId) {
    AABB2D resBox(0.f);
    ET_SendEventReturn(resBox, targetId, &ETUIElementGeom::ET_getBox);
    return resBox;
}

float convertScrollProgress(float progress, const UIScrollAreaStyle& style) {
    float v = style.origin == UIScrollOrigin::End ? 1.f - progress : progress;
    return Math::Clamp(v, 0.f, 1.f);
}

} // namespace

void UIScrollArea::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIScrollArea>("UIScrollArea")) {
        classInfo->addField("extraZOffset", &UIScrollArea::extraZOffset);
        classInfo->addField("style", &UIScrollArea::style);
        classInfo->addField("target", &UIScrollArea::targetId);
        classInfo->addField("kinematicScroll", &UIScrollArea::kinematicScrollEnabled);
    }
}

UIScrollArea::UIScrollArea() :
    scrollProgress(0.f),
    extraZOffset(1),
    isPressed(false),
    kinematicScrollEnabled(true) {
}

UIScrollArea::~UIScrollArea() {
}

void UIScrollArea::init() {
    if(targetId == getEntityId()) {
        LogWarning("[UIScrollArea::init] Can't have host element '%s' as a scroll target",
            EntityUtils::GetEntityName(targetId));
        targetId = InvalidEntityId;
    }

    ETNode<ETUIScrollArea>::connect(getEntityId());
    ETNode<ETUIInteractionBox>::connect(getEntityId());
    ETNode<ETUIElementEvents>::connect(getEntityId());
    ETNode<ETUIElemAligner>::connect(getEntityId());

    isPressed = false;

    ET_SendEvent(targetId, &ETUIElement::ET_setHostLayout, getEntityId());
    alignTarget();
}

void UIScrollArea::deinit() {
}

void UIScrollArea::alignTarget() {
    resetMoveState();

    if(!targetId.isValid()) {
        return;
    }
    ET_setScrollProgress(0.f);

    int childZIndex = 0;
    ET_SendEventReturn(childZIndex, getEntityId(), &ETUIElement::ET_getZIndex);
    childZIndex += extraZOffset;

    ET_SendEvent(targetId, &ETUIElement::ET_setZIndex, childZIndex);
}

EInputEventResult UIScrollArea::ET_onInputEvent(EActionType type, const Vec2i& pt) {
    Vec2 eventPt = Vec2(static_cast<float>(pt.x), static_cast<float>(pt.y));
    switch(type) {
        case EActionType::Press: {
            onPress(eventPt);
            break;
        }
        case EActionType::Move: {
            if(!onMove(eventPt)) {
                return EInputEventResult::Ignore;
            }
            break;
        }
        case EActionType::Release: {
            onRelease(eventPt);
            break;
        }
        case EActionType::ReleaseAndIgnore: {
            resetMoveState();
            break;
        }
    }
    return EInputEventResult::Accept;
}

void UIScrollArea::onPress(const Vec2& pt) {
    moveState.frameShift = 0;
    moveState.vel = 0.f;
    moveState.acc = 0.f;
    moveState.force = 0.f;

    isPressed = true;

    Transform tm;
    ET_SendEventReturn(tm, targetId, &ETEntity::ET_getTransform);

    moveState.destPt = Vec2(tm.pt.x, tm.pt.y);

    ETNode<ETUITimerEvents>::connect(getEntityId());

    path.clear();
    path.push_back({TimePoint::GetNow(), pt});
}

void UIScrollArea::onRelease(const Vec2& pt) {
    isPressed = false;
    auto box = getUIBox(getEntityId());
    auto clampPt = box.clamp(pt);
    path.push_back({TimePoint::GetNow(), clampPt});
    if(kinematicScrollEnabled) {
        addReleaseImpulse();
    }
}

void UIScrollArea::addReleaseImpulse() {
    assert(!path.empty() && "Empty path");

    auto& firstEvent = path.front();
    auto& lastEvent = path.back();

    auto posDt = lastEvent.pt - firstEvent.pt;
    float endVel = 0.f;

    float dt = lastEvent.timeP.getSecDeltaWith(firstEvent.timeP);

    dt = std::max(dt, 0.008f);

    if(style.type == UIScrollType::Horizontal) {
        endVel = posDt.x / dt;
    } else {
        endVel = posDt.y / dt;
    }

    if(moveState.vel >= 0.f) {
        moveState.vel = std::max(moveState.vel, endVel);
    } else {
        moveState.vel = std::min(moveState.vel, endVel);
    }

    moveState.force = -moveState.vel * 32.f;
    moveState.acc = 0.f;

    if(style.type == UIScrollType::Horizontal) {
        moveState.destPt += Vec2(moveState.vel * 2048.f, 0.f);
    } else {
        moveState.destPt += Vec2(0.f, moveState.vel * 2048.f);
    }

    path.clear();
}

bool UIScrollArea::onMove(const Vec2& pt) {
    auto box = getUIBox(getEntityId());
    if(!box.isInside(pt)) {
        onRelease(pt);
        return false;
    } else {
        path.push_back({TimePoint::GetNow(), pt});
    }
    return true;
}

void UIScrollArea::ET_onZIndexChanged(int newZIndex) {
    int childZIndex = newZIndex + extraZOffset;
    ET_SendEvent(targetId, &ETUIElement::ET_setZIndex, childZIndex);
}

void UIScrollArea::ET_onAlphaChanged(float newAlpha) {
    ET_SendEvent(targetId, &ETUIElement::ET_setParentAlpha, newAlpha);
}

void UIScrollArea::ET_onHidden(bool flag) {
    if(flag) {
        resetMoveState();
    }
    ET_SendEvent(targetId, &ETUIElement::ET_setParentHidden, flag);
    if(!flag) {
        ET_setScrollProgress(scrollProgress);
        ET_SendEvent(getEntityId(), &ETUIScrollFocus::ET_setPendingFocus);
    }
}

void UIScrollArea::ET_onDisabled(bool flag) {
    if(flag) {
        resetMoveState();
    }
    ET_SendEvent(targetId, &ETUIElement::ET_setParentDisabled, flag);
}

void UIScrollArea::updateMoveState(float dt) {
    Vec2 newPosDt(0.f);

    if(path.size() > 1) {
        auto& firstEvent = path.front();
        auto lastEvent = path.back();

        newPosDt = lastEvent.pt - firstEvent.pt;
        float eventDt = lastEvent.timeP.getSecDeltaWith(firstEvent.timeP);
        eventDt = std::max(eventDt, 0.008f);

        float newVel = 0.f;

        if(style.type == UIScrollType::Horizontal) {
            newVel = newPosDt.x / eventDt;
        } else {
            newVel = newPosDt.y / eventDt;
        }

        path.clear();
        path.push_back(lastEvent);

        if(moveState.reachDest) {
            moveState.vel = newVel;
        } else {
            if(newVel > 0.f) {
                moveState.vel = std::max(moveState.vel, newVel);
            } else {
                moveState.vel = std::min(moveState.vel, newVel);
            }
        }
    }

    moveState.destPt += newPosDt;
    moveState.frameShift = moveState.vel * dt;

    if(!isPressed) {
        moveState.acc += moveState.force * dt;
        if(moveState.vel > 0.f) {
            moveState.vel += moveState.acc * dt;
            moveState.vel = std::max(moveState.vel, 0.001f);
        } else {
            moveState.vel += moveState.acc * dt;
            moveState.vel = std::min(moveState.vel, -0.001f);
        }
    }
}

void UIScrollArea::ET_onUITick(float dt) {
    updateMoveState(dt);

    auto targetBox = getUIBox(targetId);
    AABB2D scrollArea = ET_getScrollArea();

    moveState.destPt = scrollArea.clamp(moveState.destPt);

    Vec2 resPt = targetBox.getCenter();

    if(moveState.destPt == resPt && isPressed) {
        return;
    }

    if(style.type == UIScrollType::Horizontal) {
        resPt.x += moveState.frameShift;
    } else {
        resPt.y += moveState.frameShift;
    }

    moveState.reachDest = false;
    if(style.type == UIScrollType::Horizontal) {
        if(moveState.vel > 0.f && resPt.x >= moveState.destPt.x) {
            resPt.x = moveState.destPt.x;
            moveState.reachDest = true;
        } else if(moveState.vel < 0.f && resPt.x <= moveState.destPt.x) {
            resPt.x = moveState.destPt.x;
            moveState.reachDest = true;
        }
    } else {
        if(moveState.vel > 0.f && resPt.y >= moveState.destPt.y) {
            resPt.y = moveState.destPt.y;
            moveState.reachDest = true;
        } else if(moveState.vel < 0.f && resPt.y <= moveState.destPt.y) {
            resPt.y = moveState.destPt.y;
            moveState.reachDest = true;
        }
    }

    setPosUpdateProg(scrollArea, resPt);

    if(!isPressed && std::abs(moveState.vel) < 0.01f) {
        ETNode<ETUITimerEvents>::disconnect();
    }
}

void UIScrollArea::ET_setTarget(EntityId newTargetId) {
    if(newTargetId == getEntityId()) {
        LogError("[UIScrollArea::ET_setTarget] Can't add host as a target for a scroll area: '%s'",
            EntityUtils::GetEntityName(newTargetId));
        targetId = InvalidEntityId;
        return;
    }
    targetId = newTargetId;
    UI::CopyUIElemAttribsFromParent(getEntityId(), targetId);
    alignTarget();
}

EntityId UIScrollArea::ET_getTarget() const {
    return targetId;
}

void UIScrollArea::ET_setStyle(const UIScrollAreaStyle& newStyle) {
    style = newStyle;
    alignTarget();
}

const UIScrollAreaStyle& UIScrollArea::ET_getStyle() const {
    return style;
}

void UIScrollArea::ET_reAlign() {
    const float alignScrollProgress = convertScrollProgress(scrollProgress, style);
    ET_setScrollProgress(alignScrollProgress);
}

void UIScrollArea::ET_enableKinematicScroll(bool flag) {
    kinematicScrollEnabled = flag;
}

void UIScrollArea::setPosUpdateProg(const AABB2D& scrollArea, const Vec2& newPt) {
    float prog = 0.f;
    Vec2 size = scrollArea.getSize();
    if(style.type == UIScrollType::Horizontal) {
        if(size.x > 0.f) {
            prog = (newPt.x - scrollArea.bot.x)  / size.x;
        } else {
            prog = 1.f;
        }
    } else {
        if(size.y > 0.f) {
            prog = (newPt.y - scrollArea.bot.y) / size.y;
        } else {
            prog = 1.f;
        }
    }

    scrollProgress = prog;
    UI::Set2DPos(targetId, newPt);
}

float UIScrollArea::ET_getScrollProgress() const {
    return convertScrollProgress(scrollProgress, style);
}

void UIScrollArea::ET_setScrollProgress(float newScrollProgress) {
    resetMoveState();

    scrollProgress = convertScrollProgress(newScrollProgress, style);
    auto scrollArea = ET_getScrollArea();

    Vec2 resPt =  Math::Lerp(scrollArea.bot, scrollArea.top, scrollProgress);
    setPosUpdateProg(scrollArea, resPt);
}

void UIScrollArea::ET_setTargetPosClamped(const Vec2& newScrollPt) {
    resetMoveState();

    auto scrollArea = ET_getScrollArea();
    auto resPt = scrollArea.clamp(newScrollPt);

    setPosUpdateProg(scrollArea, resPt);
}

AABB2D UIScrollArea::ET_getScrollArea() const {
    AABB2D targetBox(0.f);
    ET_SendEventReturn(targetBox, targetId, &ETUIElementGeom::ET_getBox);

    AABB2D scrollBox = getUIBox(getEntityId());
    AABB2D scrollArea = calcScrollArea(style.type, scrollBox, targetBox);

    return scrollArea;
}

void UIScrollArea::resetMoveState() {
    moveState.destPt = Vec2(0.f);
    moveState.vel = 0.f;
    moveState.acc = 0.f;
    moveState.force = 0.f;
    moveState.frameShift = 0;
    moveState.reachDest = true;

    path.clear();
    isPressed = false;

    ETNode<ETUITimerEvents>::disconnect();
}