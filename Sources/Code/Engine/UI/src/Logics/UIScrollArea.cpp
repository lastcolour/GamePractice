#include "UIScrollArea.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntity.hpp"
#include "UIUtils.hpp"

void UIScrollArea::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIScrollArea>("UIScrollArea")) {
        classInfo->addField("style", &UIScrollArea::style);
        classInfo->addField("target", &UIScrollArea::scrollElemId);
    }
}

UIScrollArea::UIScrollArea() :
    currentProg(0.f),
    startPt(0),
    endPt(0) {
}

UIScrollArea::~UIScrollArea() {
}

bool UIScrollArea::init() {
    if(scrollElemId.isValid()) {
        ETNode<ETUIInteractionBox>::connect(getEntityId());
        ETNode<ETUIElementEvents>::connect(getEntityId());
        initScrollElem();
    }
    return true;
}

void UIScrollArea::initScrollElem() {
    ET_SendEvent(scrollElemId, &ETUILayout::ET_setHostBox, getEntityId());

    AABB2Di elemBox(Vec2i(0), Vec2i(0));
    ET_SendEventReturn(elemBox, scrollElemId, &ETUILayout::ET_getCombinedBox);

    AABB2Di box = ET_getHitBox();

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
    path.push_back(pt);
}

void UIScrollArea::onRelease(const Vec2i& pt) {
    path.clear();
}

void UIScrollArea::onMove(const Vec2i& pt) {
    auto& prevPt = path.back();
    auto dir = pt - prevPt;
    if(dir.y < 1.f) {
        return;
    }

    Transform tm;
    ET_SendEventReturn(tm, scrollElemId, &ETEntity::ET_getTransform);
    tm.pt.y += dir.y;
    ET_SendEvent(scrollElemId, &ETEntity::ET_setTransform, tm);
    path.push_back(pt);
}

void UIScrollArea::ET_onBoxResized(const AABB2Di& newAabb) {
    ET_SendEvent(scrollElemId, &ETUIElementEvents::ET_onBoxResized, newAabb);
}

void UIScrollArea::ET_onZIndexChanged(int newZIndex) {
    ET_SendEvent(scrollElemId, &ETUIElementEvents::ET_onZIndexChanged, newZIndex);
}

void UIScrollArea::ET_onAlphaChanged(float newAlpha) {
    ET_SendEvent(scrollElemId, &ETUIElementEvents::ET_onAlphaChanged, newAlpha);
}

void UIScrollArea::ET_onHidden(bool flag) {
    ET_SendEvent(scrollElemId, &ETUIElementEvents::ET_onHidden, flag);
}

void UIScrollArea::ET_onDisabled(bool flag) {
    ET_SendEvent(scrollElemId, &ETUIElementEvents::ET_onDisabled, flag);
}

void UIScrollArea::ET_onIngoreTransform(bool flag) {
    ET_SendEvent(scrollElemId, &ETUIElementEvents::ET_onIngoreTransform, flag);
}