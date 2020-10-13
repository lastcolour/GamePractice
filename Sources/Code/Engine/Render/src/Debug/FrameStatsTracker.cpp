#include "Debug/FrameStatsTracker.hpp"
#include "Render/ETDebugRender.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/StringFormat.hpp"
#include "Render/ETRenderCamera.hpp"

#include <algorithm>

FrameStatsTracker::FrameStatsTracker() :
    fpsValue(0.f) {
}

FrameStatsTracker::~FrameStatsTracker() {
}

void FrameStatsTracker::onFrameStart() {
    auto currTime = TimePoint::GetNowTime();
    auto frameTime = currTime.getMiliSecElapsedFrom(frameStartT);
    frameTime = std::max(1.f, frameTime);

    frameStartT = currTime;
    fpsValue = 1000.f / frameTime;
}

void FrameStatsTracker::onFrameEnd() {
    auto currTime = TimePoint::GetNowTime();
    auto drawTime = currTime.getMiliSecElapsedFrom(frameStartT);

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETRenderCamera::ET_getRenderPort);

    Vec2i drawPt = viewPort;
    drawPt.x -= 80;
    drawPt.y -= 18;

    ColorB col(255, 255, 0);

    {
        std::string fpsText = StringFormat("FPS: %.2f", fpsValue);
        ET_SendEvent(&ETDebugRender::ET_drawText, drawPt, 16.f, col, fpsText.c_str());
    }

    drawPt.x -= 55;
    drawPt.y -= 16;

    {
        std::string drawTimeText = StringFormat("Draw Time: %.2f ms ", drawTime);
        ET_SendEvent(&ETDebugRender::ET_drawText, drawPt, 14.f, col, drawTimeText.c_str());
    }
}