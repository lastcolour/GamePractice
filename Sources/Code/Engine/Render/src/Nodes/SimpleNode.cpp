#include "Nodes/SimpleNode.hpp"
#include "Logics/RenderAuxFunctions.hpp"

SimpleNode::SimpleNode() :
    size(100),
    color(255, 255, 255) {
}

SimpleNode::~SimpleNode() {
}

void SimpleNode::onInit() {
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
    Mat4 mvp = Render::CalcModelMat(tm, Vec3(scale, 1.f), *geom);
    mvp = ctx.proj2dMat * mvp;

    mat->setUniformMat4("MVP", mvp);
    mat->setUniform4f("color", color);
    geom->draw();
}