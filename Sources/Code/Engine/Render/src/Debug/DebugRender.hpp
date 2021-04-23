#ifndef __DEBUG_RENDER_HPP__
#define __DEBUG_RENDER_HPP__

#include "Core/SystemLogic.hpp"
#include "Render/ETDebugRender.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Debug/DeubgCommands.hpp"
#include "Nodes/SimpleNode.hpp"
#include "Nodes/TextNode.hpp"
#include "Nodes/LineNode.hpp"

class DebugRender : public SystemLogic,
    public ETNode<ETDebugRender>,
    public ETNode<ETRenderContextEvents> {
public:

    DebugRender();
    virtual ~DebugRender();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETDebugRender
    void ET_drawLine(const Vec2& startPt, const Vec2& endPt, const ColorB& col) override;
    void ET_drawQuadSolid(const AABB2D& box, const ColorB& col) override;
    void ET_drawQuadBorder(const AABB2D& box, const ColorB& col) override;
    void ET_drawText(const Vec2& pt, float size, const ColorB& col, const char* text) override;
    void ET_drawCicleBorder(const Vec2& pt, float r, const ColorB& col) override;
    void ET_drawCicleSolid(const Vec2& pt, float r, const ColorB& col) override;
    void ET_update(RenderContext& ctx) override;

    // ETRenderContextEvents
    void ET_onContextCreated() override;
    void ET_onContextDestroyed() override;

private:

    void drawLines(RenderContext& ctx);
    void drawQuads(RenderContext& ctx);
    void drawTexts(RenderContext& ctx);

private:

    std::vector<RenderLine> drawLinesCmd;
    std::vector<DebugDrawQuadCmd> drawQuadCmds;
    std::vector<DebugDrawTextCmd> drawTextCmds;
    LineNode lineNode;
    SimpleNode quadNode;
    TextNode textNode;
};

#endif /* __DEBUG_RENDER_HPP__ */