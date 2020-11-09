#include "Debug/DebugRender.hpp"
#include "Render/ETRenderManager.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Render/ETRenderInterfaces.hpp"

#include <limits>
#include <cmath>
#include <algorithm>

DebugRender::DebugRender() {
}

DebugRender::~DebugRender() {
}

bool DebugRender::init() {
    ETNode<ETDebugRender>::connect(getEntityId());

    bool hasContext = false;
    ET_SendEventReturn(hasContext, &ETRender::ET_hasContext);
    if(hasContext) {
        ET_init();
    }

    return true;
}

void DebugRender::ET_init() {
    simpleNode.initRender();
    simpleNode.ET_setVisible(true);

    textNode.initRender();
    textNode.ET_setVisible(true);
    textNode.setAlignAtCenter(false);
}

void DebugRender::deinit() {
}

void DebugRender::ET_drawLine(const Vec2& startPt, const ColorB& startCol, const Vec2& endPt, const ColorB& endCol, float width) {
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

void DebugRender::ET_drawQuad(const AABB2D& box, const ColorB& col) {
    std::lock_guard<std::mutex> lock(mutex);
    {
        DebugDrawQuadCmd cmd;
        cmd.box = box;
        cmd.col = col;
        drawQuadCmds.push_back(cmd);
    }
}

void DebugRender::ET_drawText(const Vec2& pt, float size, const ColorB& col, const char* text) {
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
    RenderContext ctx;
    ET_SendEventReturn(ctx.proj2dMat, &ETRenderCamera::ET_getProj2DMat4);

    drawLines(ctx);
    drawQuads(ctx);
    drawTexts(ctx);
}

void DebugRender::drawLines(RenderContext& ctx) {
    std::lock_guard<std::mutex> lock(mutex);

    Transform tm;
    for(auto& cmd : drawLineCmds) {
        Vec2 center = (cmd.startPt + cmd.endPt) / 2.f;
        Vec2 dir = cmd.endPt - cmd.startPt;
        float lineLen = dir.getLenght();
        dir.normalize();

        tm.pt = Vec3(center.x, center.y, 0.f);
        tm.quat.setAxisAngle(Vec3(0.f, 0.f, 1.f), acos(dir.x));

        simpleNode.ET_setSize(Vec2i(lineLen, cmd.width));
        simpleNode.ET_setColor0(cmd.startCol);
        simpleNode.ET_setTransform(tm);
        simpleNode.render(ctx);
    }
    drawLineCmds.clear();
}

void DebugRender::drawQuads(RenderContext& ctx) {
    std::lock_guard<std::mutex> lock(mutex);

    Transform tm;
    for(auto& cmd : drawQuadCmds) {
        tm.pt = Vec3(cmd.box.getCenter(), 0.f);
        simpleNode.ET_setTransform(tm);
        Vec2 size = cmd.box.getSize();
        simpleNode.ET_setSize(Vec2i(size.y, size.y));
        simpleNode.ET_setColor0(cmd.col);
        simpleNode.render(ctx);
    }
    drawQuadCmds.clear();
}

void DebugRender::drawTexts(RenderContext& ctx) {
    std::lock_guard<std::mutex> lock(mutex);

    Transform tm;
    for(auto& cmd : drawTextCmds) {
        tm.pt = Vec3(static_cast<float>(cmd.pt.x), static_cast<float>(cmd.pt.y), 0.f);
        textNode.ET_setTransform(tm);
        textNode.ET_setText(cmd.text);
        textNode.ET_setColor0(cmd.col);
        textNode.ET_setFontHeight(static_cast<int>(cmd.size));
        textNode.render(ctx);
    }
    drawTextCmds.clear();
}