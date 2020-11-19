#include "Nodes/ColoredTextureNode.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Render/ETRenderManager.hpp"
#include "RenderTexture.hpp"

ColoredTextureNode::ColoredTextureNode() :
    size(100.f),
    color(255, 255, 255) {
}

ColoredTextureNode::~ColoredTextureNode() {
}

void ColoredTextureNode::onInit() {
    setShader("tex_solid_color");
    setBlendingMode(RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA);
    setGeometry(PrimitiveGeometryType::Sqaure_Tex);
}

void ColoredTextureNode::onRender(RenderContext& ctx) {
    auto scale = Render::CalcGeomScaleForSize(size, *geom);
    Mat4 modelMat = Render::CalcModelMat(tm, Vec3(scale, 1.f), *geom);

    shader->setUniformMat4(UniformType::ModelMat, modelMat);
    shader->setTexture2D(UniformType::Texture, *tex);
    shader->setUniform4f(UniformType::Color, color);

    geom->draw();
}

bool ColoredTextureNode::isVisible() const {
    if(!tex) {
        return false;
    }
    return Node::isVisible();
}

void ColoredTextureNode::setColor0(const ColorB& newColor) {
    color = newColor;
}

void ColoredTextureNode::setSize(const Vec2& newSize) {
    size = newSize;
}

void ColoredTextureNode::setImage(const std::string& newImage) {
    ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createFromImage, newImage.c_str(), ETextureType::RGBA);
}