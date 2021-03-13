#include "Debug/DebugRender.hpp"
#include "Render/ETRenderManager.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "RenderUtils.hpp"

#include <limits>
#include <cmath>
#include <algorithm>

DebugRender::DebugRender() {
}

DebugRender::~DebugRender() {
}

bool DebugRender::init() {
    ETNode<ETDebugRender>::connect(getEntityId());
    ETNode<ETRenderContextEvents>::connect(getEntityId());

    if(RenderUtils::IsOpenGLContextExists()) {
        ET_onContextCreated();
    }

    return true;
}

void DebugRender::ET_onContextCreated() {
    lineNode.init();
    lineNode.setVisible(true);

    quadNode.init();
    quadNode.setVisible(true);

    textNode.init();
    textNode.setVisible(true);
    textNode.setAlignAtCenter(false);
}

void DebugRender::ET_onContextDestroyed() {
}

void DebugRender::deinit() {
}

void DebugRender::ET_drawLine(const Vec2& startPt, const Vec2& endPt, const ColorB& col, float width) {
    DebugDrawLineCmd cmd;
    cmd.startPt = startPt;
    cmd.endPt = endPt;
    cmd.col = col;
    cmd.width = width;
    drawLineCmds.push_back(cmd);
}

void DebugRender::ET_drawQuadSolid(const AABB2D& box, const ColorB& col) {
    DebugDrawQuadCmd cmd;
    cmd.box = box;
    cmd.col = col;
    drawQuadCmds.push_back(cmd);
}

void DebugRender::ET_drawQuadBorder(const AABB2D& box, const ColorB& col, float width) {
    DebugDrawLineCmd cmd;
    cmd.col = col;
    cmd.width = width;
    {
        cmd.startPt = Vec2(box.bot.x, box.bot.y);
        cmd.endPt = Vec2(box.bot.x, box.top.y);
        drawLineCmds.push_back(cmd);
    }
    {
        cmd.startPt = Vec2(box.bot.x, box.top.y);
        cmd.endPt = Vec2(box.top.x, box.top.y);
        drawLineCmds.push_back(cmd);
    }
    {
        cmd.startPt = Vec2(box.top.x, box.top.y);
        cmd.endPt = Vec2(box.top.x, box.bot.y);
        drawLineCmds.push_back(cmd);
    }
    {
        cmd.startPt = Vec2(box.top.x, box.bot.y);
        cmd.endPt = Vec2(box.bot.x, box.bot.y);
        drawLineCmds.push_back(cmd);
    }
}

void DebugRender::ET_drawText(const Vec2& pt, float size, const ColorB& col, const char* text) {
    DebugDrawTextCmd cmd;
    cmd.text = text;
    cmd.pt = pt;
    cmd.col = col;
    cmd.size = size;
    drawTextCmds.push_back(cmd);
}

void DebugRender::ET_update(RenderContext& ctx) {
    ET_SendEvent(&ETDebugInfoProvider::ET_drawDebugInfo);

    drawLines(ctx);
    drawQuads(ctx);
    drawTexts(ctx);
}

void DebugRender::drawLines(RenderContext& ctx) {
    for(auto& cmd : drawLineCmds) {
        lineNode.setLine(cmd.startPt, cmd.endPt, cmd.col, cmd.width);
        lineNode.render(ctx);
    }
    drawLineCmds.clear();
}

void DebugRender::drawQuads(RenderContext& ctx) {
    Transform tm;
    for(auto& cmd : drawQuadCmds) {
        tm.pt = Vec3(cmd.box.getCenter(), 0.f);
        quadNode.setTransform(tm);
        Vec2 size = cmd.box.getSize();
        quadNode.setSize(Vec2(size.x, size.y));
        quadNode.setColor0(cmd.col);
        quadNode.render(ctx);
    }
    drawQuadCmds.clear();
}

void DebugRender::drawTexts(RenderContext& ctx) {
    Transform tm;
    for(auto& cmd : drawTextCmds) {
        tm.pt = Vec3(static_cast<float>(cmd.pt.x), static_cast<float>(cmd.pt.y), 0.f);
        textNode.setTransform(tm);
        textNode.setText(cmd.text);
        textNode.setColor0(cmd.col);
        textNode.setFontHeight(static_cast<int>(cmd.size));
        textNode.render(ctx);
    }
    drawTextCmds.clear();
}