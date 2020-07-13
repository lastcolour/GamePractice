#include "Logics/UIViewAppearAnimation.hpp"
#include "Reflect/ReflectContext.hpp"

void UIViewAppearAnimation::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIViewAppearAnimation>("UIViewAppearAnimation")) {
        classInfo->addField("elements", &UIViewAppearAnimation::elements);
    }
}

UIViewAppearAnimation::UIViewAppearAnimation() {
}

UIViewAppearAnimation::~UIViewAppearAnimation() {
}

bool UIViewAppearAnimation::init() {
    ETNode<ETUIViewAppearAnimation>::connect(getEntityId());
    return true;
}

void UIViewAppearAnimation::deinit() {
}

void UIViewAppearAnimation::ET_onAppTick(float dt) {
}

void UIViewAppearAnimation::ET_appear() {
}

void UIViewAppearAnimation::ET_disappear() {
}