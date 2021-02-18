#include "UIScrollArea.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntity.hpp"
#include "UIUtils.hpp"
#include "Core/ETLogger.hpp"

#include <algorithm>
#include <cassert>

namespace {

AABB2Di calcScrollArea(UIScrollType scrollType, const AABB2Di& scrollBox, const AABB2Di& targetBox) {
    auto targetSize = targetBox.getSize();
    auto scrollSize = scrollBox.getSize();
    auto scrollCenter = scrollBox.getCenter();

    AABB2Di res;

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

Vec2i clampToEdges(const AABB2Di& box, const Vec2i& pt) {
    Vec2i resPt;
    resPt.x = Math::Clamp(pt.x, box.bot.x, box.top.x);
    resPt.y = Math::Clamp(pt.y, box.bot.y, box.top.y);
    return resPt;
}

AABB2Di getUIBox(EntityId targetId) {
    AABB2Di resBox(0);
    ET_SendEventReturn(resBox, targetId, &ETUIElement::ET_getBox);
    return resBox;
}

Vec2i lerpPos(const Vec2i& from, const Vec2i& to, float t) {
    Vec2 v1(from.x, from.y);
    Vec2 v2(to.x, to.y);
    Vec2 res = Math::Lerp(v1, v2, t);
    return Vec2i(res.x, res.y);
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
    extraZOffset(0),
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

void UIScrollArea::alignTarget() {
    resetMoveState();

    if(!targetId.isValid()) {
        return;
    }
    ET_setScrollProgress(0.f);

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
        case EActionType::ReleaseAndIgnore: {
            resetMoveState();
            break;
        }
    }
    return EInputEventResult::Accept;
}

void UIScrollArea::onPress(const Vec2i& pt) {
    moveState.frameShift = 0;
    moveState.vel = 0.f;
    moveState.acc = 0.f;
    moveState.force = 0.f;

    isPressed = true;

    Transform tm;
    ET_SendEventReturn(tm, targetId, &ETEntity::ET_getTransform);

    moveState.destPt = Vec2i(static_cast<int>(tm.pt.x),
        static_cast<int>(tm.pt.y));

    ETNode<ETUITimerEvents>::connect(getEntityId());

    path.clear();
    path.push_back({TimePoint::GetNowTime(), pt});
}

void UIScrollArea::onRelease(const Vec2i& pt) {
    isPressed = false;
    auto box = getUIBox(getEntityId());
    auto clampPt = clampToEdges(box, pt);
    path.push_back({TimePoint::GetNowTime(), clampPt});
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

    float dt = lastEvent.timeP.getSecElapsedFrom(firstEvent.timeP);
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
        moveState.destPt += Vec2i(static_cast<int>(moveState.vel * 2048.f), 0);
    } else {
        moveState.destPt += Vec2i(0, static_cast<int>(moveState.vel * 2048.f));
    }

    path.clear();
}

bool UIScrollArea::onMove(const Vec2i& pt) {
    auto box = getUIBox(getEntityId());
    if(!box.isInside(pt)) {
        onRelease(pt);
        return false;
    } else {
        path.push_back({TimePoint::GetNowTime(), pt});
    }
    return true;
}

void UIScrollArea::ET_onZIndexChanged(int newZIndex) {
    auto childZIndex = UI::GetZIndexForChild(getEntityId());
    childZIndex += extraZOffset;
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

void UIScrollArea::ET_onIngoreTransform(bool flag) {
    ET_SendEvent(targetId, &ETUIElement::ET_setIgnoreTransform, flag);
}

void UIScrollArea::updateMoveState(float dt) {
    Vec2i newPosDt(0);

    if(path.size() > 1) {
        auto& firstEvent = path.front();
        auto lastEvent = path.back();

        newPosDt = lastEvent.pt - firstEvent.pt;
        float dt = lastEvent.timeP.getSecElapsedFrom(firstEvent.timeP);
        dt = std::max(dt, 0.008f);

        float newVel = 0.f;

        if(style.type == UIScrollType::Horizontal) {
            newVel = static_cast<float>(newPosDt.x) / dt;
        } else {
            newVel = static_cast<float>(newPosDt.y) / dt;
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
    moveState.frameShift = static_cast<int>(moveState.vel * dt);

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
    AABB2Di scrollArea = ET_getScrollArea();

    moveState.destPt = clampToEdges(scrollArea, moveState.destPt);

    Vec2i resPt = targetBox.getCenter();

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
    ET_setScrollProgress(scrollProgress);
}

void UIScrollArea::ET_enableKinematicScroll(bool flag) {
    kinematicScrollEnabled = flag;
}

void UIScrollArea::setPosUpdateProg(const AABB2Di& scrollArea, const Vec2i& newPt) {
    float prog = 0.f;
    Vec2i size = scrollArea.getSize();
    if(style.type == UIScrollType::Horizontal) {
        if(size.x > 0) {
            prog = (newPt.x - scrollArea.bot.x)  / static_cast<float>(size.x);
        } else {
            prog = 1.f;
        }
    } else {
        if(size.y > 0) {
            prog = (newPt.y - scrollArea.bot.y) / static_cast<float>(size.y);
        } else {
            prog = 1.f;
        }
    }

    scrollProgress = prog;
    UI::Set2DPositionDoNotUpdateLayout(targetId, newPt);
}

float UIScrollArea::ET_getScrollProgress() const {
    float resProg = scrollProgress;
    if(style.origin == UIScrollOrigin::End) {
        resProg = 1.f - scrollProgress;
    }
    return resProg;
}

void UIScrollArea::ET_setScrollProgress(float newScrollProgress) {
    resetMoveState();

    scrollProgress = newScrollProgress;
    auto scrollArea = ET_getScrollArea();

    float resProg = scrollProgress;
    if(style.origin == UIScrollOrigin::End) {
        resProg = 1.f - scrollProgress;
    }

    Vec2i resPt = lerpPos(scrollArea.bot, scrollArea.top, resProg);
    setPosUpdateProg(scrollArea, resPt);
}

void UIScrollArea::ET_setTargetPosClamped(const Vec2i& newScrollPt) {
    resetMoveState();

    auto scrollArea = ET_getScrollArea();
    auto resPt = clampToEdges(scrollArea, newScrollPt);
    setPosUpdateProg(scrollArea, resPt);
}

AABB2Di UIScrollArea::ET_getScrollArea() const {
    AABB2Di targetBox(Vec2i(0), Vec2i(0));
    ET_SendEventReturn(targetBox, targetId, &ETUIElement::ET_getBox);

    AABB2Di scrollBox = getUIBox(getEntityId());
    AABB2Di scrollArea = calcScrollArea(style.type, scrollBox, targetBox);

    return scrollArea;
}

void UIScrollArea::resetMoveState() {
    moveState.destPt = Vec2i(0);
    moveState.vel = 0.f;
    moveState.acc = 0.f;
    moveState.force = 0.f;
    moveState.frameShift = 0;
    moveState.reachDest = true;

    path.clear();
    isPressed = false;

    ETNode<ETUITimerEvents>::disconnect();
}