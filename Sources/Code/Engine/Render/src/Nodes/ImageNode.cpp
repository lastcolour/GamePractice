#include "Nodes/ImageNode.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Render/ETRenderManager.hpp"

ImageNode::ImageNode() :
    size(100) {
}

ImageNode::~ImageNode() {
}

void ImageNode::ET_setSize(const Vec2i& newSize) {
    if(Render::IsRenderThread()) {
        size = newSize;
    } else {
        ET_QueueEvent(nodeId, &ETRenderProxyNode::ET_setSize, newSize);
    }
}

void ImageNode::ET_setImage(const std::string& newImage) {
    if(Render::IsRenderThread()) {
        ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createTexture, newImage.c_str(), ETextureType::RGBA);
    } else {
        ET_QueueEvent(nodeId, &ETRenderProxyNode::ET_setImage, newImage);
    }
}

void ImageNode::onInit() {
    setBlendingMode(RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA);
    setGeometry(PrimitiveGeometryType::Sqaure_Tex);
    setMaterial("simple_image");
}

void ImageNode::onRender(RenderContext& ctx) {
    auto scale = Render::CalcGeomScaleForSize(size, *geom);
    Mat4 mvp = Render::CalcModelMat(tm, Vec3(scale, 1.f), *geom);
    mvp = ctx.proj2dMat * mvp;

    mat->setUniformMat4("MVP", mvp);
    mat->setTexture2D("tex", tex->texId);
    geom->draw();
}

bool ImageNode::isVisible() const {
    if(!tex) {
        return false;
    }
    return Node::isVisible();
}
