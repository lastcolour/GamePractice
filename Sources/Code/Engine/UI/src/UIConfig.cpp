#include "UIConfig.hpp"
#include "UI/ETUIViewPort.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Core/ETPrimitives.hpp"

#include <cmath>

void UIConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIConfig>("UIConfig")) {
        classInfo->addField("horizontalGrid", &UIConfig::horizontalGrid);
        classInfo->addField("mainView", &UIConfig::mainView);
        classInfo->addField("gameView", &UIConfig::gameView);
        classInfo->addField("backgroundView", &UIConfig::backgroundView);
        classInfo->addField("endGameView", &UIConfig::endGameView);
    }
}

UIConfig::UIConfig() :
    horizontalGrid(200) {
}

UIConfig::~UIConfig() {
}

int UIConfig::getSizeOnGrind(float value) const {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    float pixelsPerValue = viewPort.y / static_cast<float>(horizontalGrid);
    return static_cast<int>(ceil(pixelsPerValue * value));
}