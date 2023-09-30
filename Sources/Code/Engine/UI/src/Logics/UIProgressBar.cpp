#include "Logics/UIProgressBar.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/ETUILayout.hpp"
#include "UI/ETUIView.hpp"

void UIProgressBar::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIProgressBar>("UIProgressBar")) {
        (void)classInfo;
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

    Transform tm = getLocalTransform();

    tm.scale.x = Math::Lerp(0.f, 1.f, currProgress);
    setLocalTransform(tm);

    EntityId hostLayoutId;
    ET_SendEventReturn(hostLayoutId, getEntityId(), &ETUIElement::ET_getHostLayout);
    if(hostLayoutId.isValid()) {
        ET_SendEvent(&ETUIReAlignManager::ET_setLayoutDirty, getEntityId());
    }
}

float UIProgressBar::ET_getProgress() const {
    return currProgress;
}