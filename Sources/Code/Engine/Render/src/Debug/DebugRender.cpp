#include "Debug/DebugRender.hpp"
#include "Render/ETRenderManager.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Math/MatrixTransform.hpp"
#include "RenderUtils.hpp"

#include <cassert>

DebugRender::DebugRender() {
}

DebugRender::~DebugRender() {
}

bool DebugRender::init() {
    ETNode<ETRenderContextEvents>::connect(getEntityId());

    if(RenderUtils::IsOpenGLContextExists()) {
        ET_onContextCreated();
    }

    return true;
}

void DebugRender::ET_onContextCreated() {
    lineNode.init();
    lineNode.setVisible(true);
    lineNode.setBlendingMode(BlendMode{BlendType::SRC_ALPHA, BlendType::ONE_MINUS_SRC_ALPHA});
    if(!lineNode.canRender()) {
        LogError("[DebugRender::ET_onContextCreated] Can't init line node");
        return;
    }

    quadNode.init();
    quadNode.setVisible(true);
    quadNode.setBlendingMode(BlendMode{BlendType::SRC_ALPHA, BlendType::ONE_MINUS_SRC_ALPHA});
    if(!quadNode.canRender()) {
        LogError("[DebugRender::ET_onContextCreated] Can't int quat line node");
        return;
    }

    textNode.init();
    textNode.setVisible(true);
    textNode.setAlignAtCenter(false);
    textNode.setText(" ");
    textNode.setBlendingMode(BlendMode{BlendType::SRC_ALPHA, BlendType::ONE_MINUS_SRC_ALPHA});
    if(!textNode.canRender()) {
        LogError("[DebugRender::ET_onContextCreated] Can't init text line node");
        return;
    }

    ETNode<ETDebugRender>::connect(getEntityId());
}

void DebugRender::ET_onContextDestroyed() {
}

void DebugRender::deinit() {
}

void DebugRender::ET_drawLine(const Vec2& startPt, const Vec2& endPt, const ColorB& col) {
    auto colF = col.getColorF();
    drawLinesCmd.emplace_back(RenderLine{startPt, colF, endPt, colF});
}

void DebugRender::ET_drawQuadSolid(const AABB2D& box, const ColorB& col) {
    DebugDrawQuadCmd cmd;
    cmd.box = box;
    cmd.col = col;
    drawQuadCmds.push_back(cmd);
}

void DebugRender::ET_drawQuadBorder(const AABB2D& box, const ColorB& col) {
    ET_drawLine(Vec2(box.bot.x, box.bot.y), Vec2(box.bot.x, box.top.y), col);
    ET_drawLine(Vec2(box.bot.x, box.top.y), Vec2(box.top.x, box.top.y), col);
    ET_drawLine(Vec2(box.top.x, box.top.y), Vec2(box.top.x, box.bot.y), col);
    ET_drawLine(Vec2(box.top.x, box.bot.y), Vec2(box.bot.x, box.bot.y), col);
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
    lineNode.renderLines(drawLinesCmd, ctx);
    drawLinesCmd.clear();
}

void DebugRender::drawQuads(RenderContext& ctx) {
    Transform tm;
    for(auto& cmd : drawQuadCmds) {
        tm.pt = Vec3(cmd.box.getCenter(), 0.f);
        quadNode.setTransform(tm);
        Vec2 size = cmd.box.getSize();
        quadNode.setSize(Vec2(size.x, size.y));
        quadNode.setColor(cmd.col);
        quadNode.render(ctx);
    }
    drawQuadCmds.clear();
}

void DebugRender::drawTexts(RenderContext& ctx) {
    Transform tm;
    for(auto& cmd : drawTextCmds) {
        tm.pt = Vec3(static_cast<float>(cmd.pt.x), static_cast<float>(cmd.pt.y), 0.f);
        textNode.setTransform(tm);
        textNode.setText(std::move(cmd.text));
        textNode.setColor0(cmd.col);
        textNode.setFontHeight(cmd.size);
        textNode.render(ctx);
    }
    drawTextCmds.clear();
}

void DebugRender::ET_drawCicleBorder(const Vec2& pt, float r, const ColorB& col) {
    Vec2 p1(0.f);
    Vec2 p2(0.f);
    const int N = 64;
    for(int i = 0; i < N; ++i) {
        float t1 = i * 2.f * Math::PI / static_cast<float>(N);
        p1 = pt + r * Vec2(cos(t1), sin(t1));
        float t2 = (i + 1) * 2.f * Math::PI / static_cast<float>(N);
        p2 = pt + r * Vec2(cos(t2), sin(t2));
        ET_drawLine(p1, p2, col);
    }
}

void DebugRender::ET_drawCircleArc(const Vec2& pt, float r, const Vec2& dir, float angle, const ColorB& col) {
    angle = Math::Clamp(angle, -2.f * Math::PI, 2.f * Math::PI);

    Vec2 p1(0.f);
    Vec2 p2(0.f);

    p1 = pt + r * Math::RotateVec2D(dir, -angle / 2.f);
    ET_drawLine(pt, p1, col);

    p1 = pt + r * Math::RotateVec2D(dir, angle / 2.f);
    ET_drawLine(pt, p1, col);

    const int N  = 64;
    for(int i = 0; i < N; ++i) {
        p1 = pt + r * Math::RotateVec2D(dir, angle * (-0.5f + i / static_cast<float>(N)));
        p2 = pt + r * Math::RotateVec2D(dir, angle * (-0.5f + (i + 1) / static_cast<float>(N)));
        ET_drawLine(p1, p2, col);
    }
}

void DebugRender::ET_drawCicleSolid(const Vec2& pt, float r, const ColorB& col) {
    assert(false && "Not implemented");
}