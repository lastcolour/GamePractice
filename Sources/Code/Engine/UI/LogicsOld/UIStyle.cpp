#include "UI/UIStyle.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Reflect/ReflectContext.hpp"

BoxMargin::BoxMargin() :
    left(0.f),
    right(0.f),
    bot(0.f),
    top(0.f) {}

void BoxMargin::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<BoxMargin>("BoxMargin")) {
        classInfo->addField("left", &BoxMargin::left);
        classInfo->addField("right", &BoxMargin::right);
        classInfo->addField("bot", &BoxMargin::bot);
        classInfo->addField("top", &BoxMargin::top);
    }
}

UIBoxStyle::UIBoxStyle() :
    size(0.5f),
    xAlignType(XAlignType::Center),
    yAlignType(YAlignType::Center),
    margin() {
}

UIBoxStyle::~UIBoxStyle() {
}

void UIBoxStyle::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<XAlignType>("XAlignType")) {
        enumInfo->addValues<XAlignType>({
            {"Center", XAlignType::Center},
            {"Left", XAlignType::Left},
            {"Right", XAlignType::Right}
        });
    }
    if(auto enumInfo = ctx.enumInfo<YAlignType>("YAlignType")) {
        enumInfo->addValues<YAlignType>({
            {"Center", YAlignType::Center},
            {"Top", YAlignType::Top},
            {"Bot", YAlignType::Bot}
        });
    }
    if(auto classInfo = ctx.classInfo<UIBoxStyle>("UIBoxStyle")) {
        classInfo->addField("size", &UIBoxStyle::size);
        classInfo->addField("xAlignType", &UIBoxStyle::xAlignType);
        classInfo->addField("yAlignType", &UIBoxStyle::yAlignType);
    }
}

void UILabelStyle::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UILabelStyle>("UILabelStyle")) {
        classInfo->addField("color", &UILabelStyle::color);
        classInfo->addField("fontSize", &UILabelStyle::fontSize);
    }
}

UILabelStyle::UILabelStyle() :
    color(255, 255, 255),
    fontSize(20.f) {
}

UILabelStyle::~UILabelStyle() {
}