#include "Nodes/ColoredTextureNode.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Render/ETRenderManager.hpp"
#include "RenderTexture.hpp"

ColoredTextureNode::ColoredTextureNode() :
    size(100),
    color(255, 255, 255) {
}

ColoredTextureNode::~ColoredTextureNode() {
}

void ColoredTextureNode::onInit() {
    setMaterial("tex_solid_color");
    setBlendingMode(RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA);
    setGeometry(PrimitiveGeometryType::Sqaure_Tex);
}

void ColoredTextureNode::onRender(RenderContext& ctx) {
    auto scale = Render::CalcGeomScaleForSize(size, *geom);
    Mat4 mvp = Render::CalcModelMat(tm, Vec3(scale, 1.f), *geom);
    mvp = ctx.proj2dMat * mvp;

    mat->setUniformMat4("MVP", mvp);
    mat->setTexture2D("tex", tex->texId);
    mat->setUniform4f("color", color);
    geom->draw();
}

bool ColoredTextureNode::isVisible() const {
    if(!tex) {
        return false;
    }
    return Node::isVisible();
}

void ColoredTextureNode::ET_setColor0(const ColorB& newColor) {
    color = newColor;
}

void ColoredTextureNode::ET_setSize(const Vec2i& newSize) {
    size = newSize;
}

void ColoredTextureNode::ET_setImage(const std::string& newImage) {
    ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createTexture, newImage.c_str(), ETextureType::RGBA);
}