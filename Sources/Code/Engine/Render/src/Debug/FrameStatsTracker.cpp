#include "Debug/FrameStatsTracker.hpp"
#include "Render/ETDebugRender.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/StringFormat.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Core/ETLogger.hpp"

#include <algorithm>
#include <limits>

namespace {

void DrawFPSChart(Vec2& pt, const Vec2& size, const CycleArray<float>& fpsValues) {
    Vec2 startPt(0.f);
    Vec2 endPt(0.f);

    startPt = pt;
    endPt = Vec2(startPt.x, startPt.y + size.y);
    ET_SendEvent(&ETDebugRender::ET_drawLine, startPt, endPt, ColorB(255), 1.f);

    startPt = pt;
    endPt = Vec2(startPt.x + size.x, startPt.y);
    ET_SendEvent(&ETDebugRender::ET_drawLine, startPt, endPt, ColorB(255), 1.f);

    startPt = pt + size;
    endPt = Vec2(startPt.x - size.x, startPt.y);
    ET_SendEvent(&ETDebugRender::ET_drawLine, startPt, endPt, ColorB(255), 1.f);

    startPt = pt + size;
    endPt = Vec2(startPt.x, startPt.y - size.y);
    ET_SendEvent(&ETDebugRender::ET_drawLine, startPt, endPt, ColorB(255), 1.f);

    if(fpsValues.size() <= 1) {
        return;
    }

    float step = size.x / fpsValues.maxSize();

    float maxFps = 60.f;
    float maxY = size.y * 0.8f;

    float yVal = Math::Lerp(0.f, maxY, fpsValues[0] / maxFps);
    yVal = std::min(size.y * 0.99f, yVal);
    endPt.y = pt.y + yVal;
    endPt.x = pt.x + size.x;

    for(size_t i = 1, sz = fpsValues.size(); i < sz; ++i) {
        yVal = Math::Lerp(0.f, maxY, fpsValues[i] / maxFps);
        yVal = std::min(size.y * 0.99f, yVal);
        startPt.x = endPt.x - step;
        startPt.y = pt.y + yVal;
        ET_SendEvent(&ETDebugRender::ET_drawLine, startPt, endPt, ColorB(255, 255, 0), 1.f);
        endPt = startPt;
    }
}

} // namespace

FrameStatsTracker::FrameStatsTracker() :
    fpsValues(120) {
}

FrameStatsTracker::~FrameStatsTracker() {
}

void FrameStatsTracker::onFrameStart() {
    auto currTime = TimePoint::GetNowTime();
    auto frameTime = currTime.getMiliSecElapsedFrom(frameStartT);
    frameTime = std::max(1.f, frameTime);

    frameStartT = currTime;
    auto val = 1000.f / frameTime;
    fpsValues.insert(val);
}

void FrameStatsTracker::onFrameEnd() {
    auto currTime = TimePoint::GetNowTime();
    auto drawTime = currTime.getMiliSecElapsedFrom(frameStartT);

    float avgFps = 0.f;
    float minFps = std::numeric_limits<float>::max();
    float maxFps = std::numeric_limits<float>::min();
    if(fpsValues.size() > 0) {
        for(size_t i = 0, sz = fpsValues.size(); i < sz; ++i) {
            auto val = fpsValues[i];
            avgFps += val;
            if(minFps > val) {
                minFps = val;
            }
            if(maxFps < val) {
                maxFps = val;
            }
        }
        avgFps /= fpsValues.size();
    } else {
        avgFps = 0.f;
        minFps = 0.f;
        maxFps = 0.f;
    }

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETRenderCamera::ET_getRenderPort);

    Vec2 drawPt = Vec2(static_cast<float>(viewPort.x), static_cast<float>(viewPort.y));
    drawPt.x = 10.f;
    drawPt.y -= 18.f;

    ColorB col(255, 255, 0);

    {
        std::string fpsText = StringFormat("FPS: %.1f [%.1f .. %.1f]", avgFps, minFps, maxFps);
        ET_SendEvent(&ETDebugRender::ET_drawText, drawPt, 16.f, col, fpsText.c_str());
    }

    drawPt.y -= 16.f;

    {
        std::string drawTimeText = StringFormat("Draw Time: %.2f ms ", drawTime);
        ET_SendEvent(&ETDebugRender::ET_drawText, drawPt, 14.f, col, drawTimeText.c_str());
    }

    drawPt.y -= 45.f;

    DrawFPSChart(drawPt, Vec2(120.f, 40.f), fpsValues);
}