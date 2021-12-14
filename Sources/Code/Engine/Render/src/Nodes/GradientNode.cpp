#include "Nodes/GradientNode.hpp"
#include "RenderTexture.hpp"
#include "Render/ETRenderManager.hpp"

#include <cassert>

GradientNode::GradientNode() :
    size(100.f),
    startCol(255, 255, 255),
    endCol(0, 0, 0),
    isVertical(true),
    doUpdate(true) {
}

GradientNode::~GradientNode() {
}

void GradientNode::onInit() {
    setGeometry(PrimitiveGeometryType::Sqaure_Tex);
    setShader("tex_vert_color");
    ET_SendEventReturn(tex, &ETRenderTextureManager::ET_createTexture, ETextureDataType::RGBA);
    if(tex) {
        tex->bind();
        tex->resize(Vec2i(2));
        tex->setLerpType(ETextureLerpType::Point, ETextureLerpType::Point);
        tex->setWrapType(ETextureWrapType::ClampToEdge, ETextureWrapType::ClampToEdge);
        tex->unbind();
        doUpdate = true;
    } else {
        assert(false && "Can't create 2x2 texture for gradient");
    }
}

void GradientNode::onRender(RenderContext& ctx) {
    if(doUpdate) {
        doUpdate = false;
        tex->bind();
        updateTexData();
        tex->unbind();
    }
    shader->setUniformMat4(UniformType::ModelMat, modelMat);
    shader->setTexture2d(UniformType::Texture, 0, *tex);
    geom->drawTriangles();
}

bool GradientNode::canRender() const {
    if(!tex) {
        return false;
    }
    return Node::canRender();
}

void GradientNode::setVertical(bool flag) {
    isVertical = flag;
    doUpdate = true;
}

void GradientNode::setColor0(const ColorB& newColor) {
    startCol = newColor;
    doUpdate = true;
}

void GradientNode::setColor1(const ColorB& newColor) {
    endCol = newColor;
    doUpdate = true;
}

void GradientNode::setSize(const Vec2& newSize) {
    size = newSize;
    setModelMatDirty();
}

Mat4 GradientNode::calcModelMat(const Transform& newTm) {
    return RenderUtils::CalcModelMat(newTm, Vec3(size.x, size.y, 1.f));
}

void GradientNode::updateTexData() {
    if(!tex) {
        return;
    }

}