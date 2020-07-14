#include "Logics/UIViewAppearAnimation.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntity.hpp"
#include "Math/Primitivies.hpp"
#include "UI/ETUIBox.hpp"
#include "Core/ETApplication.hpp"
#include "UIConfig.hpp"

#include <algorithm>

void UIViewAppearAnimation::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIViewAppearAnimation>("UIViewAppearAnimation")) {
        classInfo->addField("elements", &UIViewAppearAnimation::elements);
    }
}

UIViewAppearAnimation::UIViewAppearAnimation() :
    animDuration(0.f),
    state(State::None) {
}

UIViewAppearAnimation::~UIViewAppearAnimation() {
}

bool UIViewAppearAnimation::init() {
    ETNode<ETUIViewAppearAnimation>::connect(getEntityId());
    ET_appear();
    return true;
}

void UIViewAppearAnimation::deinit() {
    if(state == State::None) {
        return;
    }
    ET_onAppTick(1024.f);
}

void UIViewAppearAnimation::ET_onAppTick(float dt) {
    auto elemsInProgress = elements.size();
    animDuration += dt;
    for(auto& elem : elements) {
        if(!elem.elemId.isValid()) {
            --elemsInProgress;
            continue;
        }
        auto elemTime = animDuration - elem.startDelay;
        if(elemTime < 0.f) {
            continue;
        }
        if(elem.isHidded) {
            ET_SendEvent(elem.elemId, &ETUIElement::ET_show);
            elem.isHidded = false;
        }
        if(elemTime >= elem.duration) {
            --elemsInProgress;
        }
        auto elemProg = std::min(1.f, elemTime / elem.duration);
        if(state == State::Disappear) {
            elemProg = 1.f - elemProg;
        }
        updateTransform(elem, elemProg);
        updateAlpha(elem, elemProg);
    }
    if(!elemsInProgress) {
        ET_SendEvent(&ETUIViewAppearAnimationEvents::ET_onViewAppeared, getEntityId());
        ET_SendEvent(getEntityId(), &ETUILayout::ET_setIgnoreUpdates, false);
        ET_SendEvent(getEntityId(), &ETUILayout::ET_update);
        ETNode<ETAppTimerEvents>::disconnect();
        state = State::None;
    }
}

void UIViewAppearAnimation::updateAlpha(UIViewAppearAnimationElement& elem, float prog) {
    auto resAlpha = Math::Lerp(elem.startAlpha, 1.f, prog);
    ET_SendEvent(elem.elemId, &ETUIElement::ET_setAlpha, resAlpha);
}

void UIViewAppearAnimation::updateTransform(UIViewAppearAnimationElement& elem, float prog) {
    auto uiConfig = ET_getShared<UIConfig>();
    auto resTm = elem.origTm;

    auto offset = uiConfig->getSizeOnGrind(elem.xMoveOffset);
    resTm.pt.x -= Math::Lerp(static_cast<float>(offset), 0.f, prog);

    offset = uiConfig->getSizeOnGrind(elem.yMoveOffset);
    resTm.pt.y += Math::Lerp(static_cast<float>(offset), 0.f, prog);

    resTm.scale *= Math::Lerp(elem.startScale, 1.f, prog);
    ET_SendEvent(elem.elemId, &ETEntity::ET_setTransform, resTm);
}

void UIViewAppearAnimation::ET_appear() {
    animDuration = 0.f;
    state = State::Appear;
    ETNode<ETAppTimerEvents>::connect(getEntityId());
    for(auto& elem : elements) {
        if(!elem.elemId.isValid()) {
            continue;
        }
        ET_SendEventReturn(elem.origTm, elem.elemId, &ETEntity::ET_getTransform);
        ET_SendEvent(elem.elemId, &ETUIElement::ET_hide);
        elem.isHidded = true;
    }
    ET_SendEvent(getEntityId(), &ETUILayout::ET_setIgnoreUpdates, true);
    ETNode<ETAppTimerEvents>::connect(getEntityId());
}

void UIViewAppearAnimation::ET_disappear() {
}