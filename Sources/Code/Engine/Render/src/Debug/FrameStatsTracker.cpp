#include "Debug/FrameStatsTracker.hpp"
#include "Render/ETDebugRender.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/StringFormat.hpp"
#include "Render/ETRenderCamera.hpp"

#include <algorithm>
#include <limits>

FrameStatsTracker::FrameStatsTracker() :
    fpsValues(60) {
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

    Vec2i drawPt = viewPort;
    drawPt.x -= 175;
    drawPt.y -= 18;

    ColorB col(255, 255, 0);

    {
        std::string fpsText = StringFormat("FPS: %.1f [%.1f .. %.1f]", avgFps, minFps, maxFps);
        ET_SendEvent(&ETDebugRender::ET_drawText, drawPt, 16.f, col, fpsText.c_str());
    }

    drawPt.x += 30;
    drawPt.y -= 16;

    {
        std::string drawTimeText = StringFormat("Draw Time: %.2f ms ", drawTime);
        ET_SendEvent(&ETDebugRender::ET_drawText, drawPt, 14.f, col, drawTimeText.c_str());
    }
}