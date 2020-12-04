#include "Nodes/NinePatchNode.hpp"
#include "Logics/RenderAuxFunctions.hpp"

NinePatchNode::NinePatchNode() {
}

NinePatchNode::~NinePatchNode() {
}

void NinePatchNode::onInit() {
    setShader("simple_image");
    setGeometry(PrimitiveGeometryType::NinePatch);
    setBlendingMode(RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA);
}

void NinePatchNode::setPatches(float newLeft, float newRight, float newTop, float newDown) {
    left = newLeft;
    right = newRight;
    top = newTop;
    down = newDown;
}

void NinePatchNode::onRender(RenderContext& ctx) {
    auto scale = Render::CalcGeomScaleForSize(size, *geom);
    Mat4 modelMat = Render::CalcModelMat(tm, Vec3(scale, 1.f), *geom);

    shader->setUniformMat4(UniformType::ModelMat, modelMat);
    shader->setTexture2D(UniformType::Texture, *tex);
    shader->setUniform4f(UniformType::Color, tintColor);

    geom->drawNinePatch(left, right, top, down);

    geom->draw();
}