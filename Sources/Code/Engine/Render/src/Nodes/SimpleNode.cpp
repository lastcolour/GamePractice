#include "Nodes/SimpleNode.hpp"

SimpleNode::SimpleNode() :
    size(100.f),
    color(255, 255, 255) {
}

SimpleNode::~SimpleNode() {
}

void SimpleNode::onInit() {
    setGeometry(PrimitiveGeometryType::Square);
    setShader("geom_solid_color");
}

void SimpleNode::setColor(const ColorB& newColor) {
    color = newColor;
}

void SimpleNode::setSize(const Vec2& newSize) {
    size = newSize;
    setModelMatDirty();
}

Mat4 SimpleNode::calcModelMat(const Transform& newTm) {
    return RenderUtils::CalcModelMat(newTm, Vec3(size.x, size.y, 1.f));
}

void SimpleNode::onRender(RenderContext& ctx) {
    shader->setUniformMat4(UniformType::ModelMat, modelMat);
    shader->setUniform4f(UniformType::Color, color);
    geom->drawTriangles();
}