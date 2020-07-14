#include "Logics/UIViewAppearAnimationElement.hpp"
#include "Reflect/ReflectContext.hpp"

void UIViewAppearAnimationElement::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIViewAppearAnimationElement>("UIViewAppearAnimationElement")) {
        classInfo->addField("element", &UIViewAppearAnimationElement::elemId);
        classInfo->addField("startDelay", &UIViewAppearAnimationElement::startDelay);
        classInfo->addField("duration", &UIViewAppearAnimationElement::duration);
        classInfo->addField("xMoveOffset", &UIViewAppearAnimationElement::xMoveOffset);
        classInfo->addField("yMoveOffset", &UIViewAppearAnimationElement::yMoveOffset);
        classInfo->addField("startScale", &UIViewAppearAnimationElement::startScale);
        classInfo->addField("startAlpha", &UIViewAppearAnimationElement::startAlpha);
    }
}

UIViewAppearAnimationElement::UIViewAppearAnimationElement() :
    startDelay(0.f),
    duration(0.3f),
    xMoveOffset(0.f),
    yMoveOffset(30.f),
    startScale(1.5f),
    startAlpha(0.0f),
    isHidded(false) {
}

UIViewAppearAnimationElement::~UIViewAppearAnimationElement() {
}
