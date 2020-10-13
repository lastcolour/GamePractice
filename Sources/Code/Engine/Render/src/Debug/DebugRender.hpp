#ifndef __DEBUG_RENDER_HPP__
#define __DEBUG_RENDER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Render/ETDebugRender.hpp"
#include "Debug/DeubgCommands.hpp"
#include "Nodes/SimpleNode.hpp"
#include "Nodes/TextNode.hpp"

#include <vector>
#include <mutex>

class DebugRender : public SystemLogic,
    public ETNode<ETDebugRender> {
public:

    DebugRender();
    virtual ~DebugRender();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETDebugRender
    void ET_drawLine(const Vec2i& startPt, const ColorB& startCol, const Vec2i& endPt, const ColorB& endCol, int width) override;
    void ET_drawQuad(const AABB2Di& box, const ColorB& col) override;
    void ET_drawText(const Vec2i& pt, float size, const ColorB& col, const char* text) override;
    void ET_update() override;

private:

    void drawLines(RenderContext& ctx);
    void drawQuads(RenderContext& ctx);
    void drawTexts(RenderContext& ctx);

private:

    std::mutex mutex;
    std::vector<DebugDrawLineCmd> drawLineCmds;
    std::vector<DebugDrawQuadCmd> drawQuadCmds;
    std::vector<DebugDrawTextCmd> drawTextCmds;
    SimpleNode simpleNode;
    TextNode textNode;
};

#endif /* __DEBUG_RENDER_HPP__ */