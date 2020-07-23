#include "Logics/RenderImageLogic.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "RenderTexture.hpp"
#include "RenderMaterial.hpp"
#include "RenderGeometry.hpp"
#include "RenderContext.hpp"
#include "Render/ETRenderManager.hpp"
#include "Render/ETRenderManager.hpp"
#include "Reflect/ReflectContext.hpp"

RenderImageLogic::RenderImageLogic() :
    size(100) {
    
    setBlendingMode(RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA);
}

RenderImageLogic::~RenderImageLogic() {
}

void RenderImageLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderImageLogic>("RenderImage")) {
        classInfo->addBaseClass<RenderNode>();
        classInfo->addField("size", &RenderImageLogic::size);
        classInfo->addResourceField("image", ResourceType::Image, &RenderImageLogic::ET_setImage);
    }
}

bool RenderImageLogic::init() {
    RenderNode::init();
    ET_setGeometry(PrimitiveGeometryType::Sqaure_Tex);
    if(!geom) {
        return false;
    }
    ET_setMaterial("simple_image");
    if(!mat) {
        return false;
    }
    ETNode<ETRenderImageLogic>::connect(getEntityId());
    ETNode<ETRenderRect>::connect(getEntityId());
    return true;
}

void RenderImageLogic::onRender(RenderContext& renderCtx) {
    auto scale = Render::CalcGeomScaleForSize(size, *geom);
    Mat4 mvp = Render::CalcModelMat(getEntityId(), Vec3(scale, 1.f), *geom);
    mvp = renderCtx.proj2dMat * mvp;

    mat->setUniformMat4("MVP", mvp);
    mat->setTexture2D("tex", tex->texId);
    geom->draw();
}

void RenderImageLogic::ET_setImage(const char* imageName) {
    if(!imageName || !imageName[0]) {
        tex.reset();
    } else {
        ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createTexture, imageName, ETextureType::RGBA);
    }
}

Vec2i RenderImageLogic::ET_getImageSize() const {
    if(tex) {
        return tex->size;
    }
    return Vec2i(0);
}

void RenderImageLogic::ET_setSize(const Vec2i& newSize) {
    size = newSize;
}

Vec2i RenderImageLogic::ET_getSize() const {
    return size;
}

bool RenderImageLogic::ET_isVisible() const {
    if(!tex) {
        return false;
    }
    return RenderNode::ET_isVisible();
}
