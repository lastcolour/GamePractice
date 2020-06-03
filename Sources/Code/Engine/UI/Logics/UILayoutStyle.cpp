#include "UI/Logics/UILayoutStyle.hpp"
#include "Reflect/ReflectContext.hpp"

void UILayoutStyle::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<UILayoutType>("UILayoutType")) {
        enumInfo->addValues<UILayoutType>({
            {"Horizontal", UILayoutType::Horizontal},
            {"Vertical", UILayoutType::Vertical}
        });
    }
    if(auto enumInfo = ctx.enumInfo<UIXAlign>("UIXAlign")) {
        enumInfo->addValues<UIXAlign>({
            {"Left", UIXAlign::Left},
            {"Center", UIXAlign::Center},
            {"Right", UIXAlign::Right},
        });
    }
    if(auto enumInfo = ctx.enumInfo<UIYAlign>("UIYAlign")) {
        enumInfo->addValues<UIYAlign>({
            {"Top", UIYAlign::Top},
            {"Center", UIYAlign::Center},
            {"Bot", UIYAlign::Bot},
        });
    }
    if(auto classInfo = ctx.classInfo<UILayoutStyle>("UILayoutStyle")) {
        classInfo->addField("type", &UILayoutStyle::type);
        classInfo->addField("xAlign", &UILayoutStyle::xAlign);
        classInfo->addField("yAlign", &UILayoutStyle::yAlign);
    }
}

UILayoutStyle::UILayoutStyle() :
    type(UILayoutType::Horizontal),
    xAlign(UIXAlign::Center),
    yAlign(UIYAlign::Center) {
}

UILayoutStyle::~UILayoutStyle() {
}