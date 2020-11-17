#include "Nodes/SimpleNode.hpp"
#include "Logics/RenderAuxFunctions.hpp"

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

void SimpleNode::setColor0(const ColorB& newColor) {
    color = newColor;
}

void SimpleNode::setSize(const Vec2& newSize) {
    size = newSize;
}

void SimpleNode::onRender(RenderContext& ctx) {
    auto scale = Render::CalcGeomScaleForSize(size, *geom);
    Mat4 modelMat = Render::CalcModelMat(tm, Vec3(scale, 1.f), *geom);

    shader->setUniformMat4(UniformType::ModelMat, modelMat);
    shader->setUniform4f(UniformType::Color, color);
    geom->draw();
}