#include "UILabelStyle.hpp"
#include "Reflect/ReflectContext.hpp"

void UILabelStyle::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UILabelStyle>("UILabelStyle")) {
        classInfo->addField("fontSize", &UILabelStyle::fontSize);
        classInfo->addField("margin", &UILabelStyle::margin);
    }
}

UILabelStyle::UILabelStyle() :
    fontSize(10.f) {
}

UILabelStyle::~UILabelStyle() {
}