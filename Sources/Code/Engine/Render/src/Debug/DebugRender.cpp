#include "Debug/DebugRender.hpp"

DebugRender::DebugRender() {
}

DebugRender::~DebugRender() {
}

bool DebugRender::init() {
    ETNode<ETDebugRender>::connect(getEntityId());
    return true;
}

void DebugRender::deinit() {
}

void DebugRender::ET_drawLine(const Vec2i& startPt, const ColorB& startCol, const Vec2i& endPt, const ColorB& endCol, int width) {
    std::lock_guard<std::mutex> lock(mutex);
    {
        DebugDrawLineCmd cmd;
        cmd.startPt = startPt;
        cmd.startCol = startCol;
        cmd.endPt = endPt;
        cmd.endCol = endCol;
        cmd.width = width;
        drawLineCmds.push_back(cmd);
    }
}

void DebugRender::ET_drawQuad(const AABB2Di& box, const ColorB& col) {
    std::lock_guard<std::mutex> lock(mutex);
    {
        DebugDrawQuadCmd cmd;
        cmd.box = box;
        cmd.col = col;
        drawQuadCmds.push_back(cmd);
    }
}

void DebugRender::ET_drawText(const Vec2i& pt, float size, const ColorB& col, const char* text) {
    std::lock_guard<std::mutex> lock(mutex);
    {
        DebugDrawTextCmd cmd;
        cmd.text = text;
        cmd.pt = pt;
        cmd.col = col;
        cmd.size = size;
        drawTextCmds.push_back(cmd);
    }
}

void DebugRender::ET_update() {
}