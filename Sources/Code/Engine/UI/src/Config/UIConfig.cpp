#include "Config/UIConfig.hpp"
#include "UI/ETUIViewPort.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Core/ETPrimitives.hpp"

#include <cmath>

void UIConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIConfig>("UIConfig")) {
        classInfo->addField("verticalGrid", &UIConfig::verticalGrid);
        classInfo->addField("views", &UIConfig::views);
    }
}

UIConfig::UIConfig() :
    verticalGrid(200) {
}

UIConfig::~UIConfig() {
}

int UIConfig::getSizeOnGrind(float value) const {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    float pixelsPerValue = viewPort.y / static_cast<float>(verticalGrid);
    return static_cast<int>(ceil(pixelsPerValue * value));
}