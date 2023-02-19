#include "Debug/TasksStatsTracker.hpp"
#include "Render/ETDebugRender.hpp"
#include "Render/ETRenderCamera.hpp"
#include "RenderConfig.hpp"
#include "Parallel/TasksRunner.hpp"
#include "Math/Primitivies.hpp"

namespace {

const float MAX_FPS = 120.f;
const float MAX_RUN_TIME = 48.f;
const float GRAPH_TIME_MS = 2000.f;
const float FONT_SIZE = 14.f;
const float NEW_LINE_OFFSET = 2.f;

void DrawFPSChart(DebugInfoDrawer& dd, const Vec2& pt, const Vec2& size,
    const Core::CycleArray<TaskRunInfo::StartEndTime>& timings) {

    if(timings.size() < 2) {
        return;
    }

    float prevFPSVal = -1.f;
    float xOffset = 0.f;

    for(int i = static_cast<int>(timings.size()) - 1; i >= 1; --i) {
        float runTime = std::max(0.1f, GetRunTimeBetween(timings, i));
        float fpsVal = 1000.f / runTime;

        Vec2 startPt;
        startPt.x = size.x - xOffset;
        startPt.y = std::min(1.f, fpsVal / MAX_FPS) * size.y;

        if(prevFPSVal > 0.f && !Math::IsEqual(fpsVal, prevFPSVal, 1.f)) {
            Vec2 prevEndPt = startPt;
            prevEndPt.y = std::min(1.f, prevFPSVal / MAX_FPS) * size.y;
            dd.drawLine(startPt + pt, prevEndPt + pt, ColorB(255, 255, 0));
        }

        prevFPSVal = fpsVal;

        xOffset += (runTime / GRAPH_TIME_MS) * size.x;

        Vec2 endPt;
        endPt.x = size.x - xOffset;
        endPt.y = startPt.y;

        bool lastVal = false;
        if(endPt.x < 0.f) {
            endPt.x = 0.f;
            lastVal = true;
        }
        dd.drawLine(startPt + pt, endPt + pt, ColorB(255, 255, 0));

        if(lastVal) {
            break;
        }
    }
}

void DrawRunTimeChart(DebugInfoDrawer& dd, const Vec2& pt, const Vec2& size,
    const Core::CycleArray<TaskRunInfo::StartEndTime>& timings) {

    if(timings.size() < 2) {
        return;
    }

    float prevRunDuration = -1.f;
    float xOffset = 0.f;

    for(int i = static_cast<int>(timings.size()) - 1; i >= 1; --i) {
        float runDuration = GetRunDurationAt(timings, i);
        float runTime = GetRunTimeBetween(timings, i);

        Vec2 startPt;
        startPt.x = size.x - xOffset;
        startPt.y = std::min(1.f, runDuration / MAX_RUN_TIME) * size.y;

        if(prevRunDuration > 0.f && !Math::IsEqual(runDuration, prevRunDuration, 1.f)) {
            Vec2 prevEndPt = startPt;
            prevEndPt.y = std::min(1.f, prevRunDuration / MAX_RUN_TIME) * size.y;
            dd.drawLine(startPt + pt, prevEndPt + pt, ColorB(255, 0, 255));
        }

        prevRunDuration = runDuration;

        xOffset += (runTime / GRAPH_TIME_MS) * size.x;

        Vec2 endPt;
        endPt.x = size.x - xOffset;
        endPt.y = startPt.y;

        bool lastVal = false;
        if(endPt.x < 0.f) {
            endPt.x = 0.f;
            lastVal = true;
        }
        dd.drawLine(startPt + pt, endPt + pt, ColorB(255, 0, 255));

        if(lastVal) {
            break;
        }
    }
}

void DrawGraphBox(DebugInfoDrawer& dd, const Vec2& pt, const Vec2& size) {
    auto startPt = pt;
    auto endPt = Vec2(startPt.x, startPt.y + size.y);
    dd.drawLine(startPt, endPt, ColorB(255));

    startPt = pt;
    endPt = Vec2(startPt.x + size.x, startPt.y);
    dd.drawLine(startPt, endPt, ColorB(255));

    startPt = pt + size;
    endPt = Vec2(startPt.x - size.x, startPt.y);
    dd.drawLine(startPt, endPt, ColorB(255));

    startPt = pt + size;
    endPt = Vec2(startPt.x, startPt.y - size.y);
    dd.drawLine(startPt, endPt, ColorB(255));
}

void DrawTaskRunInfo(DebugInfoDrawer& dd, Vec2& pt, const std::string& taskName, const TaskRunInfo& runInfo) {
    auto avgRunInfo = GetAvgRunInfo(runInfo);
    ColorB textCol(255, 255, 0);
    {
        std::string fpsText = StringFormat("%s: FPS: %.1f [%.1f .. %.1f]", taskName,
            avgRunInfo.avgFPS, avgRunInfo.minFPS, avgRunInfo.maxFPS);
        dd.drawText(pt, FONT_SIZE, textCol, fpsText.c_str());
    }

    pt.y -= FONT_SIZE + NEW_LINE_OFFSET;

    {
        std::string runTimeText = StringFormat("%s: AVG Run Time: %.2f [%.1f .. %.1f] ms", taskName,
            avgRunInfo.avgRunTime, avgRunInfo.minRunTime, avgRunInfo.maxRunTime);
        dd.drawText(pt, FONT_SIZE, textCol, runTimeText.c_str());
    }

    pt.y -= FONT_SIZE + 40.f;

    {
        Vec2 size(140.f, 40.f);
        DrawGraphBox(dd, pt, size);
        DrawFPSChart(dd, pt + Vec2(1.f), size - Vec2(2.f), runInfo.timing);
        DrawRunTimeChart(dd, pt + Vec2(1.f), size - Vec2(2.f), runInfo.timing);
    }
}

} // namespace

TasksStatsTracker::TasksStatsTracker() {
}

TasksStatsTracker::~TasksStatsTracker() {
}

bool TasksStatsTracker::init() {
    ETNode<ETDebugInfoProvider>::connect(getEntityId());
    return true;
}

void TasksStatsTracker::deinit() {
}

void TasksStatsTracker::ET_drawDebugInfo(DebugInfoDrawer& dd) {
    auto renderConfig = Core::GetGlobal<RenderConfig>();

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETRenderCamera::ET_getRenderPort);

    Vec2 pt = Vec2(static_cast<float>(viewPort.x), static_cast<float>(viewPort.y));
    pt.x = 10.f;
    pt.y -= FONT_SIZE;

    TaskRunInfo runInfo;
    for(auto& taskName : renderConfig->debugTaskConfig.tasks) {
        if(!GetEnv()->GetTasksRunner()->getTaskRunInfo(taskName, runInfo)) {
            continue;
        }
        DrawTaskRunInfo(dd, pt, taskName, runInfo);
        pt.y -= FONT_SIZE + NEW_LINE_OFFSET;
    }
}