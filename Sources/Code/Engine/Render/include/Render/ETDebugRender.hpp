#ifndef __ET_DEBUG_RENDER_HPP__
#define __ET_DEBUG_RENDER_HPP__

#include "Math/AABB.hpp"
#include "Render/Color.hpp"

class RenderContext;

struct ETDebugRender {
    virtual ~ETDebugRender() = default;
    virtual void ET_drawLine(const Vec2& startPt, const Vec2& endPt, const ColorB& col, float width) = 0;
    virtual void ET_drawQuad(const AABB2D& box, const ColorB& col) = 0;
    virtual void ET_drawText(const Vec2& pt, float size, const ColorB& col, const char* text) = 0;
    virtual void ET_update(RenderContext& ctx) = 0;
};

#endif /* __ET_DEBUG_RENDER_HPP__ */