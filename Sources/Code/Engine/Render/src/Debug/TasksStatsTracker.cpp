#include "Debug/TasksStatsTracker.hpp"
#include "Render/ETDebugRender.hpp"
#include "Render/ETRenderCamera.hpp"
#include "RenderConfig.hpp"
#include "Core/GlobalData.hpp"
#include "Parallel/TasksRunner.hpp"
#include "Math/Primitivies.hpp"

namespace {

const float MAX_FPS = 120.f;
const float MAX_RUN_TIME = 48.f;
const float GRAPH_TIME_MS = 2000.f;

void DrawFPSChart(const Vec2& pt, const Vec2& size, const CycleArray<TaskRunInfo::StartEndTime>& timing) {
    if(timing.size() <= 2) {
        return;
    }
    size_t i = timing.size() - 1;
    Vec2 prev;
    prev.x = 0.f;
    prev.y = std::min(1.f, GetFPSAtPoint(timing, i) / MAX_FPS);
    --i;
    Vec2 curr(0.f);
    while(true) {
        curr.x = prev.x - GetRunTimeAtPoint(timing, i - 1);
        curr.y = std::min(1.f, GetFPSAtPoint(timing, i) / MAX_FPS);
        {
            Vec2 startPt;
            startPt.x = pt.x + Math::Lerp(0.f, static_cast<float>(size.x),
                (GRAPH_TIME_MS + curr.x) / GRAPH_TIME_MS);
            startPt.y = pt.y + size.y * curr.y;
            Vec2 endPt;
            endPt.x = pt.x + Math::Lerp(0.f, static_cast<float>(size.x),
                (GRAPH_TIME_MS + prev.x) / GRAPH_TIME_MS);
            endPt.y = pt.y + size.y * prev.y;
            ET_SendEvent(&ETDebugRender::ET_drawLine, startPt, endPt, ColorB(255, 255, 0), 1.f);
        }
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

void DrawRunTimeChart(const Vec2& pt, const Vec2& size, const CycleArray<TaskRunInfo::StartEndTime>& timing) {
    if(timing.size() <= 2) {
        return;
    }
    size_t i = timing.size() - 1;
    Vec2 prev;
    prev.x = 0.f;
    prev.y = std::min(1.f, GetRunTimeAtPoint(timing, i) / MAX_RUN_TIME);
    --i;
    Vec2 curr(0.f);
    while(true) {
        float runTime = GetRunTimeAtPoint(timing, i);
        curr.x = prev.x - runTime;
        curr.y = std::min(1.f, runTime / MAX_RUN_TIME);
        {
            Vec2 startPt;
            startPt.x = pt.x + Math::Lerp(0.f, static_cast<float>(size.x),
                (GRAPH_TIME_MS + curr.x) / GRAPH_TIME_MS);
            startPt.y = pt.y + size.y * curr.y;
            Vec2 endPt;
            endPt.x = pt.x + Math::Lerp(0.f, static_cast<float>(size.x),
                (GRAPH_TIME_MS + prev.x) / GRAPH_TIME_MS);
            endPt.y = pt.y + size.y * prev.y;
            ET_SendEvent(&ETDebugRender::ET_drawLine, startPt, endPt, ColorB(255, 0, 255), 1.f);
        }
        prev = curr;
        if(i == 0) {
            break;
        }
        if(curr.x < -GRAPH_TIME_MS) {
            break;
        }
        --i;
    }
}

void DrawGraphBox(const Vec2& pt, const Vec2& size) {
    auto startPt = pt;
    auto endPt = Vec2(startPt.x, startPt.y + size.y);
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
}

void DrawTaskRunInfo(Vec2& pt, const std::string& taskName, const TaskRunInfo& runInfo) {
    auto avgRunInfo = GetAvgRunInfo(runInfo);
    ColorB textCol(255, 255, 0);
    {
        std::string fpsText = StringFormat("%s: FPS: %.1f [%.1f .. %.1f]", taskName,
            avgRunInfo.avgFPS, avgRunInfo.minFPS, avgRunInfo.maxFPS);
        ET_SendEvent(&ETDebugRender::ET_drawText, pt, 16.f, textCol, fpsText.c_str());
    }

    pt.y -= 16.f;

    {
        std::string runTimeText = StringFormat("%s: AVG Run Time: %.2f [%.1f .. %.1f] ms", taskName,
            avgRunInfo.avgRunTime, avgRunInfo.minRunTime, avgRunInfo.maxRunTime);
        ET_SendEvent(&ETDebugRender::ET_drawText, pt, 14.f, textCol, runTimeText.c_str());
    }

    pt.y -= 48.f;

    {
        Vec2 size(140.f, 40.f);
        DrawGraphBox(pt, size);
        DrawFPSChart(pt, size, runInfo.timing);
        DrawRunTimeChart(pt, size, runInfo.timing);
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

void TasksStatsTracker::ET_drawDebugInfo() {
    auto renderConfig = GetGlobal<RenderConfig>();

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETRenderCamera::ET_getRenderPort);

    Vec2 pt = Vec2(static_cast<float>(viewPort.x), static_cast<float>(viewPort.y));
    pt.x = 10.f;
    pt.y -= 18.f;

    TaskRunInfo runInfo;
    for(auto& taskName : renderConfig->debugTaskConfig.tasks) {
        if(!GetEnv()->GetTasksRunner()->getTaskRunInfo(taskName, runInfo)) {
            continue;
        }
        DrawTaskRunInfo(pt, taskName, runInfo);
        pt.y -= 18.f;
    }
}