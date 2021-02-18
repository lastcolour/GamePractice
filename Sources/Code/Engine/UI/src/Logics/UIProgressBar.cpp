#include "Logics/UIProgressBar.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Math/Primitivies.hpp"
#include "Entity/ETEntity.hpp"

#include <algorithm>

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
}

float UIProgressBar::ET_getProgress() const {
    return currProgress;
}