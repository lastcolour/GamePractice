#ifndef __DEBUG_RENDER_HPP__
#define __DEBUG_RENDER_HPP__

#include "Render/ETDebugRender.hpp"
#include "Commands/Commands.hpp"

class DebugCmdCollector : public DebugInfoDrawer {
public:

    DebugCmdCollector();
    virtual ~DebugCmdCollector();

    void clearCmds();

    // DebugInfoDrawer
    void drawLine(const Vec2& startPt, const Vec2& endPt, const ColorB& col) override;
    void drawQuadSolid(const AABB2D& box, const ColorB& col) override;
    void drawQuadBorder(const AABB2D& box, const ColorB& col) override;
    void drawText(const Vec2& pt, float fontHeight, const ColorB& col, const char* text) override;
    void drawCicleBorder(const Vec2& pt, float r, const ColorB& col) override;
    void drawCicleSolid(const Vec2& pt, float r, const ColorB& col) override;
    void drawCircleArc(const Vec2& pt, float r, const Vec2& dir, float angle, const ColorB& col) override;

public:

    std::vector<DebugDrawLineCmd> drawLinesCmds;
    std::vector<DebugDrawQuadCmd> drawQuadCmds;
    std::vector<DebugDrawTextCmd> drawTextCmds;
};

#endif /* __DEBUG_RENDER_HPP__ */