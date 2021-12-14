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

void DrawLineBetweenPoints(DebugInfoDrawer& dd, const Vec2& curr, const Vec2& prev,
    const Vec2& drawPt, const Vec2& size, const ColorB& col) {
    Vec2 startPt;
    startPt.x = drawPt.x + Math::Lerp(0.f, static_cast<float>(size.x),
        (GRAPH_TIME_MS + curr.x) / GRAPH_TIME_MS);
    startPt.y = drawPt.y + (size.y - 2.f) * curr.y + 1.f;
    Vec2 endPt;
    endPt.x = drawPt.x + Math::Lerp(0.f, static_cast<float>(size.x),
        (GRAPH_TIME_MS + prev.x) / GRAPH_TIME_MS);
    endPt.y = drawPt.y + (size.y - 2.f) * prev.y + 1.f;

    dd.drawLine(startPt, endPt, col);
}

void DrawFPSChart(DebugInfoDrawer& dd, const Vec2& pt, const Vec2& size,
    const Core::CycleArray<TaskRunInfo::StartEndTime>& timing) {
    if(timing.size() <= 2) {
        return;
    }
    size_t i = timing.size() - 1;
    Vec2 prev;
    prev.x = 0.f;
    float runTime = std::max(0.1f, GetRunTimeBetween(timing, i));
    float fpsVal = 1000.f / runTime;
    prev.y = std::min(1.f, fpsVal / MAX_FPS);
    --i;
    Vec2 curr(0.f);
    while(true) {
        runTime = std::max(0.1f, GetRunTimeBetween(timing, i));
        fpsVal = 1000.f / runTime;
        curr.x = prev.x - runTime;
        curr.y = std::min(1.f, fpsVal / MAX_FPS);
        DrawLineBetweenPoints(dd, curr, prev, pt, size, ColorB(255, 255, 0));
        prev = curr;
        if(i == 1) {
            break;
        }
        if(curr.x < -GRAPH_TIME_MS) {
            break;
        }
        --i;
    }
}

void DrawRunTimeChart(DebugInfoDrawer& dd, const Vec2& pt, const Vec2& size,
    const Core::CycleArray<TaskRunInfo::StartEndTime>& timing) {
    if(timing.size() <= 2) {
        return;
    }
    size_t i = timing.size() - 1;
    Vec2 prev;
    prev.x = 0.f;
    prev.y = std::min(1.f, GetRunDurationAt(timing, i) / MAX_RUN_TIME);
    --i;
    Vec2 curr(0.f);
    while(true) {
        curr.x = prev.x - GetRunTimeBetween(timing, i);
        curr.y = std::min(1.f,  GetRunDurationAt(timing, i) / MAX_RUN_TIME);
        DrawLineBetweenPoints(dd, curr, prev, pt, size, ColorB(255, 0, 255));
        prev = curr;
        if(i == 1) {
            break;
        }
        if(curr.x < -GRAPH_TIME_MS) {
            break;
        }
        --i;
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
        DrawFPSChart(dd, pt, size, runInfo.timing);
        DrawRunTimeChart(dd, pt, size, runInfo.timing);
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