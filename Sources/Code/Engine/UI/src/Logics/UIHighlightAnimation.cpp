#include "Logics/UIHighlightAnimation.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntity.hpp"
#include "Render/ETRenderNode.hpp"
#include "UIUtils.hpp"
#include "UI/ETUIBox.hpp"

#include <algorithm>

void UIHighlightAnimation::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIHighlightAnimation>("UIHighlightAnimation")) {
        classInfo->addField("target", &UIHighlightAnimation::targetId);
        classInfo->addField("duration", &UIHighlightAnimation::duration);
        classInfo->addField("scale", &UIHighlightAnimation::scale);
    }
}

UIHighlightAnimation::UIHighlightAnimation() :
    duration(0.3f),
    scale(1.2f),
    currentDuration(0.f) {
}

UIHighlightAnimation::~UIHighlightAnimation() {
}

void UIHighlightAnimation::ET_onUITick(float dt) {
    currentDuration += dt;
    if(currentDuration > getTotalDuration()) {
        UI::SetLocalTMDoNotUpdateLayout(targetId, startTm);
        ETNode<ETUITimerEvents>::disconnect();
        ET_SendEvent(senderId, &ETUIHighlightAnimationEvents::ET_onHighlightPlayed);
        return;
    }

    float prog = 0.f; 
    if(currentDuration < duration) {
        prog = currentDuration / duration;
        prog = std::min(prog, 1.f);
    } else {
        prog = 1.f - UI::ReturnAnimScaleFactor * (currentDuration - duration) / duration;
        prog = std::max(0.f, prog);
    }

    float scaleVal = Math::Lerp(1.f, scale, prog);

    Transform tm = startTm;
    tm.scale *= scaleVal;

    UI::SetLocalTMDoNotUpdateLayout(targetId, tm);
}

void UIHighlightAnimation::ET_playHightlight(EntityId triggerId) {
    senderId = triggerId;
    currentDuration = 0.f;
    ET_SendEventReturn(startTm, targetId, &ETEntity::ET_getLocalTransform);
    ETNode<ETUITimerEvents>::connect(getEntityId());
}

float UIHighlightAnimation::getTotalDuration() const {
    return duration * (1.f + 1.f / UI::ReturnAnimScaleFactor);
}

bool UIHighlightAnimation::init() {
    ETNode<ETUIHighlightAnimation>::connect(getEntityId());
    return true;
}

void UIHighlightAnimation::deinit() {
}