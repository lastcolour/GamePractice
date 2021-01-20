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
    extraZOffset(0),
    isPressed(false),
    kinematicScrollEnabled(true) {
}

UIScrollArea::~UIScrollArea() {
}

void UIScrollArea::init() {
    ETNode<ETUIScrollArea>::connect(getEntityId());
    ETNode<ETUIInteractionBox>::connect(getEntityId());
    ETNode<ETUIElementEvents>::connect(getEntityId());
    ETNode<ETUIElemAligner>::connect(getEntityId());

    isPressed = false;
    moveState.vel = 0.f;
    moveState.destPt = Vec2i(0);
    moveState.frameShift = 0;
    moveState.accumDt = 0.f;
    moveState.acc = 0.f;

    alignTarget();
}

void UIScrollArea::alignTarget() {
    if(!targetId.isValid()) {
        return;
    }
    if(targetId == getEntityId()) {
        LogWarning("[UIScrollArea::alignTarget] Can't have host element '%s' as a scroll target",
            EntityUtils::GetEntityName(targetId));
        targetId = InvalidEntityId;
        return;
    }

    AABB2Di targetBox(Vec2i(0), Vec2i(0));
    ET_SendEventReturn(targetBox, targetId, &ETUIElement::ET_getBox);

    AABB2Di scrollBox = ET_getHitBox();
    AABB2Di scrollArea = calcScrollArea(style.type, scrollBox, targetBox);

    if(style.origin == UIScrollOrigin::Start) {
        UI::Set2DPositionDoNotUpdateLayout(targetId, scrollArea.bot);
    } else {
        UI::Set2DPositionDoNotUpdateLayout(targetId, scrollArea.top);
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
    moveState.frameShift = 0;
    moveState.accumDt = 0.f;
    moveState.vel = 0.f;
    moveState.acc = 0.f;

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
    auto box = ET_getHitBox();
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
    dt = Math::Clamp(dt, 0.001f, 0.016f);

    if(style.type == UIScrollType::Horizontal) {
        endVel = posDt.x / dt;
        moveState.destPt += Vec2i(static_cast<int>(endVel * 2048.f), 0);
    } else {
        endVel = posDt.y / dt;
        moveState.destPt += Vec2i(0, static_cast<int>(endVel * 2048.f));
    }

    if(moveState.vel * endVel > 0.f) {
        if(moveState.vel > 0.f) {
            moveState.vel = std::max(moveState.vel, endVel);
        } else {
            moveState.vel = std::min(moveState.vel, endVel);
        }
    }
    moveState.acc = -4.f * moveState.vel;

    path.clear();
}

bool UIScrollArea::onMove(const Vec2i& pt) {
    auto box = ET_getHitBox();
    if(!box.isInside(pt)) {
        onRelease(pt);
        return false;
    } else {
        path.push_back({TimePoint::GetNowTime(), pt});
    }
    return true;
}

void UIScrollArea::ET_onBoxChanged(const AABB2Di& newAabb) {
    alignTarget();
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
    if(!flag) {
        alignTarget();
    }
}

void UIScrollArea::ET_onDisabled(bool flag) {
    ET_SendEvent(targetId, &ETUIElement::ET_setParentDisabled, flag);
}

void UIScrollArea::ET_onIngoreTransform(bool flag) {
    ET_SendEvent(targetId, &ETUIElement::ET_setIgnoreTransform, flag);
}

void UIScrollArea::updateMoveState(float dt) {
    moveState.accumDt += dt;

    Vec2i newPosDt(0);

    if(path.size() > 1) {
        auto& firstEvent = path.front();
        auto lastEvent = path.back();

        newPosDt = lastEvent.pt - firstEvent.pt;
        float dt = lastEvent.timeP.getSecElapsedFrom(firstEvent.timeP);
        dt = Math::Clamp(dt, 0.001f, 0.016f);

        float newVel = 0.f;

        if(style.type == UIScrollType::Horizontal) {
            newVel = static_cast<float>(newPosDt.x) / dt;
        } else {
            newVel = static_cast<float>(newPosDt.y) / dt;
        }

        path.clear();
        path.push_back(lastEvent);

        if(newVel * moveState.vel < 0.f) {
            moveState.vel = newVel;
        } else {
            if(newVel > 0.f) {
                moveState.vel = std::max(moveState.vel, newVel);
            } else {
                moveState.vel = std::min(moveState.vel, newVel);
            }
        }
    }

    if(!isPressed) {
        if(moveState.vel > 0.f) {
            moveState.vel += moveState.acc * dt;
            moveState.vel = std::max(moveState.vel, 0.001f);
        } else {
            moveState.vel += moveState.acc * dt;
            moveState.vel = std::min(moveState.vel, -0.001f);
        }
    }

    moveState.destPt += newPosDt;
    moveState.frameShift = static_cast<int>(moveState.vel * moveState.accumDt);

    if(std::abs(moveState.frameShift) > 0) {
        moveState.accumDt -= static_cast<float>(moveState.frameShift) / moveState.vel;
    }
}

void UIScrollArea::ET_onUITick(float dt) {
    updateMoveState(dt);

    AABB2Di targetBox(Vec2i(0), Vec2i(0));
    ET_SendEventReturn(targetBox, targetId, &ETUIElement::ET_getBox);
    Vec2i resPt = targetBox.getCenter();

    AABB2Di scrollBox = ET_getHitBox();
    AABB2Di scrollArea = calcScrollArea(style.type, scrollBox, targetBox);

    moveState.destPt = clampToEdges(scrollArea, moveState.destPt);

    if(style.type == UIScrollType::Horizontal) {
        resPt.x += moveState.frameShift;
    } else {
        resPt.y += moveState.frameShift;
    }

    bool reachedDest = false;
    if(style.type == UIScrollType::Horizontal) {
        if(moveState.vel > 0.f && resPt.x >= moveState.destPt.x) {
            resPt.x = moveState.destPt.x;
            reachedDest = true;
        } else if(moveState.vel < 0.f && resPt.x <= moveState.destPt.x) {
            resPt.x = moveState.destPt.x;
            reachedDest = true;
        }
    } else {
        if(moveState.vel > 0.f && resPt.y >= moveState.destPt.y) {
            resPt.y = moveState.destPt.y;
            reachedDest = true;
        } else if(moveState.vel < 0.f && resPt.y <= moveState.destPt.y) {
            resPt.y = moveState.destPt.y;
            reachedDest = true;
        }
    }

    UI::Set2DPositionDoNotUpdateLayout(targetId, resPt);

    if(reachedDest && isPressed) {
        moveState.vel = 0.f;
    }

    if(!isPressed && std::abs(moveState.vel) < 0.01f) {
        ETNode<ETUITimerEvents>::disconnect();
    }
}

void UIScrollArea::ET_setTarget(EntityId newTargetId) {
    targetId = newTargetId;
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
    alignTarget();
}

void UIScrollArea::ET_enableKinematicScroll(bool flag) {
    kinematicScrollEnabled = flag;
}