#include "UIScrollArea.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntity.hpp"
#include "UIUtils.hpp"

void UIScrollArea::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIScrollArea>("UIScrollArea")) {
        classInfo->addField("style", &UIScrollArea::style);
        classInfo->addField("target", &UIScrollArea::targetId);
    }
}

UIScrollArea::UIScrollArea() :
    currentProg(0.f),
    currentSpeed(0.f),
    minPt(0),
    maxPt(0) {
}

UIScrollArea::~UIScrollArea() {
}

bool UIScrollArea::init() {
    ETNode<ETUIScrollArea>::connect(getEntityId());
    ETNode<ETUIInteractionBox>::connect(getEntityId());
    ETNode<ETUIElementEvents>::connect(getEntityId());
    ETNode<ETUITimerEvents>::connect(getEntityId());
    initScrollElem();
    return true;
}

void UIScrollArea::initScrollElem() {
    if(!targetId.isValid()) {
        return;
    }

    AABB2Di elemBox(Vec2i(0), Vec2i(0));
    ET_SendEventReturn(elemBox, targetId, &ETUIElement::ET_getBox);
    elemBox.top = elemBox.getSize();
    elemBox.bot = Vec2i(0);

    AABB2Di box = ET_getHitBox();
    box.top = box.getSize();
    box.bot = Vec2i(0);

    Vec2i startPt(0);
    Vec2i endPt(0);

    if(style.type == UIScrollType::Horizontal) {
        startPt = UI::CalcAligmentCenter(UIXAlign::Left, UIYAlign::Center, box, elemBox);
        endPt = UI::CalcAligmentCenter(UIXAlign::Right, UIYAlign::Center, box, elemBox);
    } else {
        startPt = UI::CalcAligmentCenter(UIXAlign::Center, UIYAlign::Top, box, elemBox);
        endPt = UI::CalcAligmentCenter(UIXAlign::Center, UIYAlign::Bot, box, elemBox);
    }

    if(style.origin == UIScrollOrigin::End) {
        std::swap(startPt, endPt);
    }

    auto size = box.getSize();
    Vec2 origCenter(size.x / 2.f, size.y / 2.f);
    Vec2 newCenter(static_cast<float>(startPt.x), static_cast<float>(startPt.y));
    Vec2 shiftPt = newCenter - origCenter;

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    tm.pt.x += shiftPt.x;
    tm.pt.y += shiftPt.y;
    ET_SendEvent(targetId, &ETEntity::ET_setTransform, tm);
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
            onMove(pt);
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
    path.clear();
    path.push_back({TimePoint::GetNowTime(), pt});
}

void UIScrollArea::onRelease(const Vec2i& pt) {
    path.push_back({TimePoint::GetNowTime(), pt});
}

void UIScrollArea::onMove(const Vec2i& pt) {
    path.push_back({TimePoint::GetNowTime(), pt});
}

void UIScrollArea::ET_onBoxResized(const AABB2Di& newAabb) {
    ET_SendEvent(targetId, &ETUIElementEvents::ET_onBoxResized, newAabb);
    initScrollElem();
}

void UIScrollArea::ET_onZIndexChanged(int newZIndex) {
    ET_SendEvent(targetId, &ETUIElementEvents::ET_onZIndexChanged, newZIndex);
}

void UIScrollArea::ET_onAlphaChanged(float newAlpha) {
    ET_SendEvent(targetId, &ETUIElementEvents::ET_onAlphaChanged, newAlpha);
}

void UIScrollArea::ET_onHidden(bool flag) {
    ET_SendEvent(targetId, &ETUIElementEvents::ET_onHidden, flag);
}

void UIScrollArea::ET_onDisabled(bool flag) {
    ET_SendEvent(targetId, &ETUIElementEvents::ET_onDisabled, flag);
}

void UIScrollArea::ET_onIngoreTransform(bool flag) {
    ET_SendEvent(targetId, &ETUIElementEvents::ET_onIngoreTransform, flag);
}

void UIScrollArea::ET_onUITick(float dt) {
    Vec2 ptShift(0.f);
    float shiftTime = 0.f;

    if(path.size() > 1) {
        auto& startEvent = path.front();
        auto& endEvent = path.back();

        auto ptDiff = endEvent.pt - startEvent.pt;

        ptShift.x = static_cast<float>(ptDiff.x);
        ptShift.y = static_cast<float>(ptDiff.y);

        shiftTime = endEvent.timeP.getSecondsElapsedFrom(startEvent.timeP);

        path.clear();
    } else {
        return;
    }

    ptShift = -ptShift;

    Transform tm;
    ET_SendEventReturn(tm, targetId, &ETEntity::ET_getTransform);

    if(style.type == UIScrollType::Horizontal) {
        tm.pt.x += ptShift.x;
    } else {
        tm.pt.y += ptShift.y;
    }

    // tm.pt.x = Math::Clamp(tm.pt.x, static_cast<float>(minPt.x), static_cast<float>(maxPt.x));
    // tm.pt.y = Math::Clamp(tm.pt.y, static_cast<float>(minPt.y), static_cast<float>(maxPt.y));

    ET_SendEvent(targetId, &ETEntity::ET_setTransform, tm);
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