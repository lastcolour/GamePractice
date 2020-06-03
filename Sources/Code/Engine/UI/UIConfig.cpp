#include "UI/UIConfig.hpp"
#include "Reflect/ReflectContext.hpp"

void UIConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIConfig>("UIConfig")) {
        classInfo->addField("horizontalGrid", &UIConfig::horizontalGrid);
    }
}

UIConfig::UIConfig() :
    horizontalGrid(200) {
}

UIConfig::~UIConfig() {
}