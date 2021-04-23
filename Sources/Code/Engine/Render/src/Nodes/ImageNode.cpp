#include "Nodes/ImageNode.hpp"
#include "Render/ETRenderManager.hpp"
#include "RenderUtils.hpp"
#include "RenderTexture.hpp"

ImageNode::ImageNode() :
    size(100.f) {
}

ImageNode::~ImageNode() {
}

void ImageNode::setSize(const Vec2& newSize) {
    size = newSize;
    setModelMatDirty();
}

void ImageNode::onInit() {
    setBlendingMode(BlendMode{BlendType::SRC_ALPHA, BlendType::ONE_MINUS_SRC_ALPHA});
    setGeometry(PrimitiveGeometryType::Sqaure_Tex);
    setShader("simple_image");
}

void ImageNode::onRender(RenderContext& ctx) {
    RenderUtils::ApplyTextureInfo(*texObj, texInfo);
    shader->setUniformMat4(UniformType::ModelMat, modelMat);
    shader->setTexture2d(UniformType::Texture, 0, *texObj);
    shader->setUniform4f(UniformType::Color, texInfo.tintColor);
    geom->drawTriangles();
}

bool ImageNode::canRender() const {
    if(!texObj) {
        return false;
    }
    return Node::canRender();
}

Mat4 ImageNode::calcModelMat(const Transform& newTm) {
    return RenderUtils::CalcModelMat(newTm, Vec3(size.x, size.y, 1.f));
}

void ImageNode::setTextureInfo(const TextureInfo& newTextureInfo) {
    texInfo = newTextureInfo;
    texObj = RenderUtils::CreateTexture(texInfo);
}

void ImageNode::setTextureObject(std::shared_ptr<RenderTexture>& newTextureObj) {
    texObj = newTextureObj;
    texInfo.filename.clear();
    if(texObj) {
        texInfo.lerpType = texObj->minLerpType;
    } 
    texInfo.tintColor = ColorB(0, 0, 0, 0);
}