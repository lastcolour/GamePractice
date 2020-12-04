#include "Nodes/ImageNode.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Render/ETRenderManager.hpp"

ImageNode::ImageNode() :
    size(100.f),
    tintColor(255, 255, 255, 0) {
}

ImageNode::~ImageNode() {
}

void ImageNode::setSize(const Vec2& newSize) {
    size = newSize;
}

void ImageNode::setImage(const std::string& newImage) {
    ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createFromImage, newImage.c_str(), ETextureType::RGBA);
}

void ImageNode::setTintColor(const ColorB& newTintColor) {
    tintColor = newTintColor;
}

void ImageNode::onInit() {
    setBlendingMode(RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA);
    setGeometry(PrimitiveGeometryType::Sqaure_Tex);
    setShader("simple_image");
}

void ImageNode::onRender(RenderContext& ctx) {
    auto scale = Render::CalcGeomScaleForSize(size, *geom);
    Mat4 modelMat = Render::CalcModelMat(tm, Vec3(scale, 1.f), *geom);

    shader->setUniformMat4(UniformType::ModelMat, modelMat);
    shader->setTexture2D(UniformType::Texture, *tex);
    shader->setUniform4f(UniformType::Color, tintColor);
    geom->draw();
}

bool ImageNode::isVisible() const {
    if(!tex) {
        return false;
    }
    return Node::isVisible();
}