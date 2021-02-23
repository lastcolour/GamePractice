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
    setModelMatDirty();
}

void ImageNode::setImage(const std::string& newImage) {
    ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createFromImage, newImage.c_str(), ETextureType::RGBA);
}

void ImageNode::setTintColor(const ColorB& newTintColor) {
    tintColor = newTintColor;
}

void ImageNode::onInit() {
    setBlendingMode(BlendMode{BlendType::SRC_ALPHA, BlendType::ONE_MINUS_SRC_ALPHA});
    setGeometry(PrimitiveGeometryType::Sqaure_Tex);
    setShader("simple_image");
}

void ImageNode::onRender(RenderContext& ctx) {
    shader->setUniformMat4(UniformType::ModelMat, modelMat);
    shader->setTexture2D(UniformType::Texture, *tex);
    shader->setUniform4f(UniformType::Color, tintColor);
    geom->drawTriangles();
}

bool ImageNode::canRender() const {
    if(!tex) {
        return false;
    }
    return Node::canRender();
}

Mat4 ImageNode::calcModelMat(const Transform& newTm) {
    return Render::CalcModelMat(newTm, Vec3(size.x, size.y, 1.f));
}

void ImageNode::setTexture(std::shared_ptr<RenderTexture>& newTex) {
    tex = newTex;
}

std::shared_ptr<RenderTexture> ImageNode::getTexture() {
    return tex;
}