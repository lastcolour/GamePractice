#include "Config/UIConfig.hpp"
#include "UI/ETUIViewPort.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Core/ETPrimitives.hpp"

#include <cmath>

namespace {

const float BASE_X_RATIO = 9.f;
const float BASE_Y_RATIO = 16.f;

} // namespace

void UIConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIConfig>("UIConfig")) {
        classInfo->addField("horizontalGrid", &UIConfig::horizontalGrid);
        classInfo->addField("views", &UIConfig::views);
    }
}

UIConfig::UIConfig() :
    horizontalGrid(200) {
}

UIConfig::~UIConfig() {
}

float UIConfig::getSizeOnGrid(float value) const {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    float xSize = std::min(static_cast<float>(viewPort.x), viewPort.y * BASE_X_RATIO / BASE_Y_RATIO);
    float pixelsPerValue = xSize / static_cast<float>(horizontalGrid);

    return pixelsPerValue * value;
}

float UIConfig::convertFromGrid(float value) const {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    float xSize = std::min(static_cast<float>(viewPort.x), viewPort.y * BASE_X_RATIO / BASE_Y_RATIO);
    float pixelsPerValue = xSize / static_cast<float>(horizontalGrid);

    return value / pixelsPerValue;
}