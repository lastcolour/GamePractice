#include "Render/Logics/RenderLinearGradientRect.hpp"
#include "Render/RenderMaterial.hpp"
#include "Render/RenderGeometry.hpp"
#include "Render/RenderTexture.hpp"
#include "Render/RenderContext.hpp"
#include "Render/Logics/RenderAuxFunctions.hpp"
#include "Core/JSONNode.hpp"

RenderLinearGradientRect::RenderLinearGradientRect() :
    scale(1.f),
    startCol(0),
    endCol(0),
    isVertical(true) {
}

RenderLinearGradientRect::~RenderLinearGradientRect() {
}

bool RenderLinearGradientRect::serialize(const JSONNode& node) {
    if(!RenderNode::serialize(node)) {
        return false;
    }
    node.read("vertical", isVertical);
    Render::ReadColor(node.object("startColor"), startCol);
    Render::ReadColor(node.object("endColor"), endCol);
    return true;
}

bool RenderLinearGradientRect::init() {
    if(!RenderNode::init()) {
        return false;
    }
    ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createEmptyTexture, Vec2i(2), ETextureType::RGBA);
    if(!tex) {
        return false;
    }
    tex->setTexLerpType(TexLerpType::Nearest, TexLerpType::Nearest);
    tex->setWrapType(TexWrapType::ClamToEdge, TexWrapType::ClamToEdge);
    updateTexData();
    if(!geom) {
        ET_setGeometry("square_tex");
        if(!geom) {
            return false;
        }
    }
    if(!mat) {
        ET_setMaterial("tex_vert_color");
        if(!mat) {
            return false;
        }
    }
    ETNode<ETRenderRect>::connect(getEntityId());
    return true;
}

void RenderLinearGradientRect::updateTexData() {
    if(isVertical) {
        ColorB vertColors[] = {
            startCol,
            startCol,
            endCol,
            endCol
        };
        tex->modifyRGBA(Vec2i(0), Vec2i(2), vertColors);
    } else {
        ColorB vertColors[] = {
            startCol,
            endCol,
            startCol,
            endCol
        };
        tex->modifyRGBA(Vec2i(0), Vec2i(2), vertColors);
    }
}

void RenderLinearGradientRect::ET_setSize(const Vec2i& newSize) {
    scale = Render::CalcGeomScaleForSize(newSize, *geom);
}

Vec2i RenderLinearGradientRect::ET_getSize() const {
    return Render::CaclScaledGeomSize(scale, *geom);
}

void RenderLinearGradientRect::ET_onRender(const RenderContext& renderCtx) {
    Mat4 mvp = Render::CalcModelMat(getEntityId(), Vec3(scale, 1.f), *geom);
    mvp = renderCtx.proj2dMat * mvp;

    mat->bind();
    mat->setUniformMat4("MVP", mvp);
    mat->setTexture2D("tex", tex->texId);
    geom->draw();
    mat->unbind();
}