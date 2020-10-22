#include "Nodes/SimpleNode.hpp"
#include "Logics/RenderAuxFunctions.hpp"

SimpleNode::SimpleNode() :
    size(100),
    color(255, 255, 255) {
}

SimpleNode::~SimpleNode() {
}

void SimpleNode::onInitRender() {
    setGeometry(PrimitiveGeometryType::Square);
    setMaterial("geom_solid_color");
}

void SimpleNode::ET_setColor0(const ColorB& newColor) {
    color = newColor;
}

void SimpleNode::ET_setSize(const Vec2i& newSize) {
    size = newSize;
}

void SimpleNode::onRender(RenderContext& ctx) {
    auto scale = Render::CalcGeomScaleForSize(size, *geom);
    Mat4 modelMat = Render::CalcModelMat(tm, Vec3(scale, 1.f), *geom);

    mat->setUniformMat4("ModelMat", modelMat);
    mat->setUniform4f("color", color);
    geom->draw();
}