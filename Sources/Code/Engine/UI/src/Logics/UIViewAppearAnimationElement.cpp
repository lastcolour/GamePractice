#include "Logics/UIViewAppearAnimationElement.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Reflect/EnumInfo.hpp"

void UIViewAppearAnimationElement::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<AppearType>("AppearType")) {
        enumInfo->addValues<AppearType>({
            {"None", AppearType::None},
            {"FromTop", AppearType::FromTop}
        });
    }
    if(auto classInfo = ctx.classInfo<UIViewAppearAnimationElement>("UIViewAppearAnimationElement")) {
        classInfo->addField("element", &UIViewAppearAnimationElement::elemId);
        classInfo->addField("startDelay", &UIViewAppearAnimationElement::startDelay);
        classInfo->addField("moveOffset", &UIViewAppearAnimationElement::moveOffset);
        classInfo->addField("duration", &UIViewAppearAnimationElement::duration);
        classInfo->addField("appearType", &UIViewAppearAnimationElement::appearType);
    }
}

UIViewAppearAnimationElement::UIViewAppearAnimationElement() :
    elemId(),
    startDelay(0.0f),
    moveOffset(10.f),
    duration(0.1f),
    appearType(AppearType::FromTop) {
}

UIViewAppearAnimationElement::~UIViewAppearAnimationElement() {
}
