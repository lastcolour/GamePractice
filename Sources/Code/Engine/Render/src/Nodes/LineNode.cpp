#include "Nodes/LineNode.hpp"
#include "Platform/OpenGL.hpp"

#include <cassert>

LineNode::LineNode() :
    startPt(0.f),
    endPt(0.f),
    col(255, 255, 255),
    width(1.f) {
}

LineNode::~LineNode() {
}

void LineNode::setLine(const Vec2& newStartPt, const Vec2& newEndPt, const ColorB& newCol, float newWidth) {
    assert(width > 0.f && "Invalid line width");

    startPt = newStartPt;
    endPt = newEndPt;
    col = newCol;
    width = newWidth;
}

Mat4 LineNode::calcModelMat(const Transform& newTm) {
    return Mat4(1.f);
}

void LineNode::onInit() {
    setGeometry(PrimitiveGeometryType::Line);
    setShader("line_solid_color");
}

void LineNode::onRender(RenderContext& ctx) {
    glLineWidth(width);

    Vec2 vertData[] = {startPt, endPt};
    shader->setUniform4f(UniformType::Color, col);
    geom->drawLine(&vertData);
}