#include "Logics/RenderLinearGradientRect.hpp"
#include "RenderMaterial.hpp"
#include "RenderGeometry.hpp"
#include "RenderTexture.hpp"
#include "RenderContext.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Render/ETRenderManager.hpp"
#include "Reflect/ReflectContext.hpp"

RenderLinearGradientRect::RenderLinearGradientRect() :
    size(20),
    startCol(0),
    endCol(0),
    isVertical(true) {
}

RenderLinearGradientRect::~RenderLinearGradientRect() {
}

void RenderLinearGradientRect::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderLinearGradientRect>("RenderLinearGradientRect")) {
        classInfo->addField("size", &RenderLinearGradientRect::size);
        classInfo->addField("startColor", &RenderLinearGradientRect::startCol);
        classInfo->addField("endColor", &RenderLinearGradientRect::endCol);
        classInfo->addField("isVertical", &RenderLinearGradientRect::isVertical);
    }
}

bool RenderLinearGradientRect::init() {
    ET_setGeometry(PrimitiveGeometryType::Sqaure_Tex);
    if(!geom) {
        return false;
    }
    ET_setMaterial("tex_vert_color");
    if(!mat) {
        return false;
    }
    ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createEmptyTexture, Vec2i(2), ETextureType::RGBA);
    if(!tex) {
        return false;
    }
    tex->setTexLerpType(TexLerpType::Nearest, TexLerpType::Nearest);
    tex->setWrapType(TexWrapType::ClamToEdge, TexWrapType::ClamToEdge);

    updateTexData();

    RenderNode::init();

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
    size = newSize;
}

Vec2i RenderLinearGradientRect::ET_getSize() const {
    return size;
}

void RenderLinearGradientRect::onRender(RenderContext& renderCtx) {
    auto scale = Render::CalcGeomScaleForSize(size, *geom);
    Mat4 mvp = Render::CalcModelMat(getEntityId(), Vec3(scale, 1.f), *geom);
    mvp = renderCtx.proj2dMat * mvp;

    mat->setUniformMat4("MVP", mvp);
    mat->setTexture2D("tex", tex->texId);
    geom->draw();
}