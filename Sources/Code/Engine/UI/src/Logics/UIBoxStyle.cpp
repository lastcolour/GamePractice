#include "UI/UIBoxStyle.hpp"
#include "Reflect/ReflectContext.hpp"

void UIBoxStyle::Margin::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIBoxStyle::Margin>("Margin")) {
        classInfo->addField("top", &UIBoxStyle::Margin::top);
        classInfo->addField("bot", &UIBoxStyle::Margin::bot);
        classInfo->addField("left", &UIBoxStyle::Margin::left);
        classInfo->addField("right", &UIBoxStyle::Margin::right);
    }
}

void UIBoxStyle::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<UIBoxSizeInvariant>("UIBoxSizeInvariant")) {
        enumInfo->addValues<UIBoxSizeInvariant>({
            {"Grid", UIBoxSizeInvariant::Grid},
            {"Relative", UIBoxSizeInvariant::Relative}
        });
    }
    if(auto classInfo = ctx.classInfo<UIBoxStyle>("UIBoxStyle")) {
        classInfo->addField("width", &UIBoxStyle::width);
        classInfo->addField("widthInv", &UIBoxStyle::widthInv);
        classInfo->addField("height", &UIBoxStyle::height);
        classInfo->addField("heightInv", &UIBoxStyle::heightInv);
        classInfo->addField("margin", &UIBoxStyle::margin);
    }
}

UIBoxStyle::UIBoxStyle() :
    width(30.f),
    height(30.f),
    widthInv(UIBoxSizeInvariant::Grid),
    heightInv(UIBoxSizeInvariant::Grid) {
}

UIBoxStyle::~UIBoxStyle() {
}

UIBoxMargin::UIBoxMargin() :
    top(0),
    bot(0),
    left(0),
    right(0) {
}

UIBoxMargin::~UIBoxMargin() {
}