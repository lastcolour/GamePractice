#include "Logics/UIHighlightAnimation.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntity.hpp"
#include "Render/ETRenderNode.hpp"
#include "UIUtils.hpp"
#include "UI/ETUIBox.hpp"

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
    if(currentDuration > ET_getDuration()) {
        UI::SetLocalTMDoNotUpdateLayout(targetId, startTm);
        ETNode<ETUITimerEvents>::disconnect();
        return;
    }

    float prog = 0.f; 
    if(currentDuration < duration) {
        prog = currentDuration / duration;
    } else {
        prog = 1.f - UI::ReturnAnimScaleFactor * (currentDuration - duration) / duration;
    }

    float scaleVal = Math::Lerp(1.f, scale, prog);

    Transform tm = startTm;
    tm.scale *= scaleVal;

    UI::SetLocalTMDoNotUpdateLayout(targetId, tm);
}

void UIHighlightAnimation::ET_start() {
    currentDuration = 0.f;
    ET_SendEvent(getEntityId(), &ETRenderNode::ET_show);

    AABB2Di box(Vec2i(0), Vec2i(0));
    ET_SendEventReturn(box, targetId, &ETUIElement::ET_getBox);
    ET_SendEventReturn(startTm, targetId, &ETEntity::ET_getLocalTransform);

    ET_SendEvent(getEntityId(), &ETEntity::ET_setTransform, startTm);

    ETNode<ETUITimerEvents>::connect(getEntityId());
}

float UIHighlightAnimation::ET_getDuration() const {
    return duration * (1.f + 1.f / UI::ReturnAnimScaleFactor);
}

bool UIHighlightAnimation::init() {
    ETNode<ETUIAnimation>::connect(getEntityId());
    return true;
}

void UIHighlightAnimation::deinit() {
}