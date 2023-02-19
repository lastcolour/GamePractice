#ifndef __ET_DEBUG_RENDER_HPP__
#define __ET_DEBUG_RENDER_HPP__

#include "Math/AABB.hpp"
#include "Math/Color.hpp"

struct DebugInfoDrawer {
    virtual ~DebugInfoDrawer() = default;
    virtual void drawLine(const Vec2& startPt, const Vec2& endPt, const ColorB& col) = 0;
    virtual void drawQuadSolid(const AABB2D& box, const ColorB& col) = 0;
    virtual void drawQuadBorder(const AABB2D& box, const ColorB& col) = 0;
    virtual void drawText(const Vec2& pt, float fontHeight, const ColorB& col, const char* text) = 0;
    virtual void drawCircleBorder(const Vec2& pt, float r, const ColorB& col) = 0;
    virtual void drawCircleSolid(const Vec2& pt, float r, const ColorB& col) = 0;
    virtual void drawCircleArc(const Vec2& pt, float r, const Vec2& dir, float angle, const ColorB& col) = 0;
};

struct ETDebugInfoProvider {
    virtual ~ETDebugInfoProvider() = default;
    virtual void ET_drawDebugInfo(DebugInfoDrawer& dd) = 0;
};

#endif /* __ET_DEBUG_RENDER_HPP__ */