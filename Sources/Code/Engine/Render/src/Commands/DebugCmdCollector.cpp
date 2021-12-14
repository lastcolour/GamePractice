#include "Commands/DebugCmdCollector.hpp"
#include "Math/MatrixTransform.hpp"

#include <cassert>

DebugCmdCollector::DebugCmdCollector() {
}

DebugCmdCollector::~DebugCmdCollector() {
}

void DebugCmdCollector::clearCmds() {
    drawLinesCmds.clear();
    drawQuadCmds.clear();
    drawTextCmds.clear();
}

void DebugCmdCollector::drawLine(const Vec2& startPt, const Vec2& endPt, const ColorB& col) {
    DebugDrawLineCmd cmd;
    cmd.startPt = startPt;
    cmd.startCol = col;
    cmd.endPt = endPt;
    cmd.endCol = col;
    drawLinesCmds.push_back(cmd);
}

void DebugCmdCollector::drawQuadSolid(const AABB2D& box, const ColorB& col) {
    DebugDrawQuadCmd cmd;
    cmd.box = box;
    cmd.col = col;
    drawQuadCmds.push_back(cmd);
}

void DebugCmdCollector::drawQuadBorder(const AABB2D& box, const ColorB& col) {
    drawLine(Vec2(box.bot.x, box.bot.y), Vec2(box.bot.x, box.top.y), col);
    drawLine(Vec2(box.bot.x, box.top.y), Vec2(box.top.x, box.top.y), col);
    drawLine(Vec2(box.top.x, box.top.y), Vec2(box.top.x, box.bot.y), col);
    drawLine(Vec2(box.top.x, box.bot.y), Vec2(box.bot.x, box.bot.y), col);
}

void DebugCmdCollector::drawText(const Vec2& pt, float fontHeight, const ColorB& col, const char* text) {
    DebugDrawTextCmd cmd;
    cmd.text = text;
    cmd.pt = pt;
    cmd.col = col;
    cmd.fontHeight = fontHeight;
    drawTextCmds.push_back(cmd);
}

void DebugCmdCollector::drawCicleBorder(const Vec2& pt, float r, const ColorB& col) {
    Vec2 p1(0.f);
    Vec2 p2(0.f);
    const int N = 64;
    for(int i = 0; i < N; ++i) {
        float t1 = i * 2.f * Math::PI / static_cast<float>(N);
        p1 = pt + r * Vec2(cos(t1), sin(t1));
        float t2 = (i + 1) * 2.f * Math::PI / static_cast<float>(N);
        p2 = pt + r * Vec2(cos(t2), sin(t2));
        drawLine(p1, p2, col);
    }
}

void DebugCmdCollector::drawCicleSolid(const Vec2& pt, float r, const ColorB& col) {
    assert(false && "Not implemented");
}

void DebugCmdCollector::drawCircleArc(const Vec2& pt, float r, const Vec2& dir, float angle, const ColorB& col) {
    angle = Math::Clamp(angle, -2.f * Math::PI, 2.f * Math::PI);

    Vec2 p1(0.f);
    Vec2 p2(0.f);

    p1 = pt + r * Math::RotateVec2D(dir, -angle / 2.f);
    drawLine(pt, p1, col);

    p1 = pt + r * Math::RotateVec2D(dir, angle / 2.f);
    drawLine(pt, p1, col);

    const int N  = 64;
    for(int i = 0; i < N; ++i) {
        p1 = pt + r * Math::RotateVec2D(dir, angle * (-0.5f + i / static_cast<float>(N)));
        p2 = pt + r * Math::RotateVec2D(dir, angle * (-0.5f + (i + 1) / static_cast<float>(N)));
        drawLine(p1, p2, col);
    }
}
