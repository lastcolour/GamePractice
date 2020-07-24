#include "UIConfig.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Render/ETRenderCamera.hpp"
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
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);
    float pixelsPerValue = renderPort.y / static_cast<float>(horizontalGrid);
    return static_cast<int>(ceil(pixelsPerValue * value));
}