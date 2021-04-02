#include "Config/UIConfig.hpp"
#include "UI/ETUIViewPort.hpp"

void UIConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIConfig>("UIConfig")) {
        classInfo->addField("horizontalGrid", &UIConfig::horizontalGrid);
        classInfo->addField("baseRatio", &UIConfig::baseRatio);
        classInfo->addField("views", &UIConfig::views);
    }
}

UIConfig::UIConfig() :
    baseRatio(9, 16),
    horizontalGrid(200) {
}

UIConfig::~UIConfig() {
}

float UIConfig::getSizeOnGrid(float value) const {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    if(viewPort.y == 0 || viewPort.x == 0) {
        return value;
    } 

    float r = baseRatio.x / static_cast<float>(baseRatio.y);
    float s = std::min(viewPort.y * r, static_cast<float>(viewPort.x)) / static_cast<float>(viewPort.x);

    return (viewPort.x * s * value) / horizontalGrid;
}

float UIConfig::convertFromGrid(float value) const {
    return value / getSizeOnGrid(1.f);
}