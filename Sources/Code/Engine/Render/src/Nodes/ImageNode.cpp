#include "Nodes/ImageNode.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Render/ETRenderManager.hpp"

ImageNode::ImageNode() :
    size(100.f) {
}

ImageNode::~ImageNode() {
}

void ImageNode::ET_setSize(const Vec2& newSize) {
    size = newSize;
}

void ImageNode::ET_setImage(const std::string& newImage) {
    ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createTexture, newImage.c_str(), ETextureType::RGBA);
}

void ImageNode::onInitRender() {
    setBlendingMode(RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA);
    setGeometry(PrimitiveGeometryType::Sqaure_Tex);
    setMaterial("simple_image");
}

void ImageNode::onRender(RenderContext& ctx) {
    auto scale = Render::CalcGeomScaleForSize(size, *geom);
    Mat4 modelMat = Render::CalcModelMat(tm, Vec3(scale, 1.f), *geom);

    mat->setUniformMat4("ModelMat", modelMat);
    mat->setTexture2D("tex", tex->texId);
    geom->draw();
}

bool ImageNode::isVisible() const {
    if(!tex) {
        return false;
    }
    return Node::isVisible();
}
