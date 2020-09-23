#include "UI/UIScrollAreaStyle.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Reflect/EnumInfo.hpp"

void UIScrollAreaStyle::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<UIScrollType>("UIScrollType")) {
        enumInfo->addValues<UIScrollType>({
            {"Horizontal", UIScrollType::Horizontal},
            {"Vertical", UIScrollType::Vertical}
        });
    }
    if(auto enumInfo = ctx.enumInfo<UIScrollOrigin>("UIScrollOrigin")) {
        enumInfo->addValues<UIScrollOrigin>({
            {"Start", UIScrollOrigin::Start},
            {"End", UIScrollOrigin::End}
        });
    }
    if(auto classInfo = ctx.classInfo<UIScrollAreaStyle>("UIScrollAreaStyle")) {
        classInfo->addField("type", &UIScrollAreaStyle::type);
        classInfo->addField("origin", &UIScrollAreaStyle::origin);
    }
}

UIScrollAreaStyle::UIScrollAreaStyle() :
    type(UIScrollType::Vertical),
    origin(UIScrollOrigin::Start) {
}

UIScrollAreaStyle::~UIScrollAreaStyle() {
}