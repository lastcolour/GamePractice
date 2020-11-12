#include "Logics/UIViewAppearAnimation.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntity.hpp"
#include "Math/Primitivies.hpp"
#include "UI/ETUIBox.hpp"
#include "Core/ETApplication.hpp"
#include "Config/UIConfig.hpp"
#include "Core/ETLogger.hpp"
#include "UI/ETUILayout.hpp"

#include <algorithm>

namespace {

const float DISAPPEAR_TIME_SCALE = 1.2f;

} // namespace

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
    return true;
}

void UIViewAppearAnimation::deinit() {
    if(state == State::None) {
        return;
    }
    ET_onUITick(1024.f);
}

void UIViewAppearAnimation::ET_onUITick(float dt) {
    auto elemsInProgress = elements.size();
    animDuration += dt;
    auto resAnimDuration = animDuration;
    if(state == State::Disappear) {
        resAnimDuration *= DISAPPEAR_TIME_SCALE;
    }
    for(auto& elem : elements) {
        if(!elem.elemId.isValid()) {
            --elemsInProgress;
            continue;
        }
        auto elemTime = resAnimDuration - elem.startDelay;
        if(elemTime < 0.f) {
            continue;
        }
        if(state == State::Appear && elem.isHidded) {
            ET_SendEvent(elem.elemId, &ETUIElement::ET_show);
            elem.isHidded = false;
        }
        if(elemTime >= elem.duration) {
            --elemsInProgress;
            if(state == State::Disappear && !elem.isHidded) {
                ET_SendEvent(elem.elemId, &ETUIElement::ET_hide);
                elem.isHidded = true;
            }
        }
        auto elemProg = std::min(1.f, elemTime / elem.duration);
        if(state == State::Disappear) {
            elemProg = 1.f - elemProg;
        }
        updateTransform(elem, elemProg);
        updateAlpha(elem, elemProg);
    }
    if(!elemsInProgress) {
        for(auto& elem : elements) {
            if(elem.elemId.isValid()) {
                ET_SendEvent(elem.elemId, &ETEntity::ET_setLocalTransform, elem.origTm);
            }
        }
        if(state == State::Appear) {
            ET_SendEvent(triggerEntId, &ETUIViewAppearAnimationEvents::ET_onAppeared, getEntityId());
        } else {
            ET_SendEvent(getEntityId(), &ETUIElement::ET_hide);
            ET_SendEvent(triggerEntId, &ETUIViewAppearAnimationEvents::ET_onDisappeared, getEntityId());
        }
        triggerEntId = InvalidEntityId;
        ET_SendEvent(getEntityId(), &ETUIElement::ET_setIgnoreTransform, false);
        ET_SendEvent(getEntityId(), &ETUIElemAligner::ET_reAlign);
        ETNode<ETUITimerEvents>::disconnect();
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
    ET_SendEvent(elem.elemId, &ETEntity::ET_setLocalTransform, resTm);
}

void UIViewAppearAnimation::ET_appear(EntityId triggerId) {
    triggerEntId = triggerId;
    animDuration = 0.f;
    state = State::Appear;
    ETNode<ETUITimerEvents>::connect(getEntityId());
    ET_SendEvent(getEntityId(), &ETUIElement::ET_show);

    for(auto& elem : elements) {
        if(!elem.elemId.isValid()) {
            LogWarning("[UIViewAppearAnimation::ET_appear] Invalid element's id");
            continue;
        }
        ET_SendEvent(elem.elemId, &ETUIElement::ET_hide);
        ET_SendEventReturn(elem.origTm, elem.elemId, &ETEntity::ET_getLocalTransform);
        elem.isHidded = true;
    }
    ET_SendEvent(getEntityId(), &ETUIElement::ET_setIgnoreTransform, true);
    ETNode<ETUITimerEvents>::connect(getEntityId());
}

void UIViewAppearAnimation::ET_disappear(EntityId triggerId) {
    triggerEntId = triggerId;
    animDuration = 0.f;
    state = State::Disappear;
    ETNode<ETUITimerEvents>::connect(getEntityId());
    for(auto& elem : elements) {
        if(!elem.elemId.isValid()) {
            LogWarning("[UIViewAppearAnimation::ET_disappear] Invalid element's id");
            continue;
        }
        bool isHidden = false;
        ET_SendEventReturn(isHidden, elem.elemId, &ETUIElement::ET_isHidden);
        if(isHidden) {
            LogWarning("[UIViewAppearAnimation::ET_disappear] Element '%s' is hidden before appear animation start",
                EntityUtils::GetEntityName(elem.elemId));
        }
        ET_SendEventReturn(elem.origTm, elem.elemId, &ETEntity::ET_getLocalTransform);
        elem.isHidded = false;
    }
    ET_SendEvent(getEntityId(), &ETUIElement::ET_setIgnoreTransform, true);
    ETNode<ETUITimerEvents>::connect(getEntityId());
}