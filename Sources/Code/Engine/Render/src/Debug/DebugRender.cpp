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

void DebugRender::ET_drawLine(const Vec2i& startPt, const ColorB& startCol, const Vec2i& endPt, const Vec2i& endCol, int width) {
}

void DebugRender::ET_drawQuad(const AABB2Di& box, const ColorB& col) {
}

void DebugRender::ET_drawText(const Vec2i& pt, float size, const char* text) {
}
