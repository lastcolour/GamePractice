#include "Debug/DebugRender.hpp"
#include "Render/ETRenderManager.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Render/ETRenderInterfaces.hpp"

#include <limits>
#include <cmath>
#include <algorithm>

namespace {

AABB2Di GetBoxFromStartEnd(const Vec2i& startPt, const Vec2i& endPt) {
    AABB2Di box;

    box.bot.x = std::min(startPt.x, endPt.x);
    box.bot.y = std::min(startPt.y, endPt.y);
    box.top.x = std::max(startPt.x, endPt.x);
    box.top.y = std::max(startPt.y, endPt.y);

    return box;
}

} // namespace

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
        auto box = GetBoxFromStartEnd(cmd.startPt, cmd.endPt);
        Vec2i center = box.getCenter();
        Vec2i size = box.getSize();
        tm.pt = Vec3(static_cast<float>(center.x), static_cast<float>(center.y), 0.f);
        simpleNode.ET_setTransform(tm);
        int lineLen = static_cast<int>(sqrtf(static_cast<float>(size.x * size.x + size.y * size.y)));
        int lineWidth = 2;
        simpleNode.ET_setSize(Vec2i(lineLen, lineWidth));
        simpleNode.ET_setColor0(cmd.startCol);
        simpleNode.render(ctx);
    }
    drawLineCmds.clear();
}

void DebugRender::drawQuads(RenderContext& ctx) {
    std::lock_guard<std::mutex> lock(mutex);

    Transform tm;
    for(auto& cmd : drawQuadCmds) {
        Vec2i center = cmd.box.getCenter();
        tm.pt = Vec3(static_cast<float>(center.x), static_cast<float>(center.y), 0.f);
        simpleNode.ET_setTransform(tm);
        simpleNode.ET_setSize(cmd.box.getSize());
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