#include "Nodes/LineNode.hpp"
#include "Platform/OpenGL.hpp"
#include "Debug/DeubgCommands.hpp"

#include <cassert>

LineNode::LineNode() {
}

LineNode::~LineNode() {
}

Mat4 LineNode::calcModelMat(const Transform& newTm) {
    return Mat4(1.f);
}

void LineNode::onInit() {
    setGeometry(PrimitiveGeometryType::Line);
    setShader("line_solid_color");
}

void LineNode::renderLines(const std::vector<RenderLine>& lines, RenderContext& ctx) {
    if(lines.empty()) {
        return;
    }
    onRenderStart(ctx);
    geom->drawLines(&lines[0], static_cast<unsigned int>(lines.size()));
    onRenderEnd(ctx);
}