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
    ET_SendEventReturn(tex, &ETRenderTextureManager::ET_createTexture, ETextureType::RGBA);
    if(tex) {
        tex->bind();
        tex->resize(Vec2i(2));
        tex->setPixelLerpType(TexLerpType::Nearest, TexLerpType::Nearest);
        tex->setPixelWrapType(TexWrapType::ClamToEdge, TexWrapType::ClamToEdge);
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
    shader->setTexture2D(UniformType::Texture, *tex);
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
    if(isVertical) {
        ColorB vertColors[] = {
            startCol,
            startCol,
            endCol,
            endCol
        };
        tex->writeRGBA(Vec2i(0), Vec2i(2), vertColors);
    } else {
        ColorB vertColors[] = {
            startCol,
            endCol,
            startCol,
            endCol
        };
        tex->writeRGBA(Vec2i(0), Vec2i(2), vertColors);
    }

    if(startCol.a < 1.f || endCol.a < 1.f) {
        setBlendingMode(BlendMode{BlendType::SRC_ALPHA, BlendType::ONE_MINUS_SRC_ALPHA});
    } else {
        setBlendingMode(BlendMode{BlendType::NONE, BlendType::NONE});
    }
}