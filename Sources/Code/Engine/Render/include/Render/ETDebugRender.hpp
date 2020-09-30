#ifndef __ET_DEBUG_RENDER_HPP__
#define __ET_DEBUG_RENDER_HPP__

#include "Math/AABB.hpp"
#include "Render/Color.hpp"

struct ETDebugRender {
    virtual ~ETDebugRender() = default;
    virtual void ET_drawLine(const Vec2i& startPt, const ColorB& startCol, const Vec2i& endPt, const Vec2i& endCol, int width) = 0;
    virtual void ET_drawQuad(const AABB2Di& box, const ColorB& col) = 0;
    virtual void ET_drawText(const Vec2i& pt, float size, const char* text) = 0;
};

#endif /* __ET_DEBUG_RENDER_HPP__ */