#include "Logics/UIProgressBar.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/ETUILayout.hpp"

void UIProgressBar::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIProgressBar>("UIProgressBar")) {
    }
}

UIProgressBar::UIProgressBar() :
    currProgress(0.f) {
}

UIProgressBar::~UIProgressBar() {
}

void UIProgressBar::init() {
    ETNode<ETUIProgressBar>::connect(getEntityId());
}

void UIProgressBar::deinit() {
}

void UIProgressBar::ET_setProgress(float newProgress) {
    currProgress = Math::Clamp(newProgress, 0.f, 1.f);

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getLocalTransform);

    tm.scale.x = Math::Lerp(0.f, 1.f, currProgress);
    ET_SendEvent(getEntityId(), &ETEntity::ET_setLocalTransform, tm);

    EntityId hostLayoutId;
    ET_SendEventReturn(hostLayoutId, getEntityId(), &ETUIElement::ET_getHostLayout);
    if(hostLayoutId.isValid()) {
        ET_SendEvent(hostLayoutId, &ETUIElemAligner::ET_reAlign);
    }
}

float UIProgressBar::ET_getProgress() const {
    return currProgress;
}