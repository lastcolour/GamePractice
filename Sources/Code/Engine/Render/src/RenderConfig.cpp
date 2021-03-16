#include "RenderConfig.hpp"

RenderConfig::RenderConfig() :
    defaultFontSize(38) {
}

void RenderConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderConfig>("RenderConfig")) {
        classInfo->addField("defaultFont", &RenderConfig::defaultFont);
        classInfo->addField("defaultFontSize", &RenderConfig::defaultFontSize);
    }
}
