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

UIStyle::UIStyle() :
    size(1.f),
    color(255, 255, 255),
    sizeInv(SizeInvariant::Relative),
    xAlignType(XAlignType::Center),
    yAlignType(YAlignType::Center),
    margin(),
    renderer(),
    fontSize(0.5f),
    fontColor(0, 0, 0) {
}

UIStyle::~UIStyle() {
}

void UIStyle::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<SizeInvariant>("SizeInvariant")) {
        enumInfo->addValues<SizeInvariant>({
            {"Relative", SizeInvariant::Relative},
            {"RelativeBiggestSquare", SizeInvariant::RelativeBiggestSquare},
            {"Absolute", SizeInvariant::Absolute},
            {"AbsoluteBiggestSquare", SizeInvariant::AbsoluteBiggestSquare},
            {"Pixel", SizeInvariant::Pixel}
        });
    }
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
    if(auto classInfo = ctx.classInfo<UIStyle>("UIStyle")) {
        classInfo->addField("size", &UIStyle::size);
        classInfo->addField("color", &UIStyle::color);
        classInfo->addField("renderer", &UIStyle::renderer);
        classInfo->addField("fontSize", &UIStyle::fontSize);
        classInfo->addField("fontColor", &UIStyle::fontColor);
        classInfo->addField("sizeInv", &UIStyle::sizeInv);
        classInfo->addField("xAlignType", &UIStyle::xAlignType);
        classInfo->addField("yAlignType", &UIStyle::yAlignType);
    }
}