#include "Render/Logics/RenderImageLogic.hpp"
#include "Render/RenderTexture.hpp"
#include "Render/RenderMaterial.hpp"
#include "Render/RenderGeometry.hpp"
#include "Entity/ETEntityInterfaces.hpp"
#include "Math/MatrixTransform.hpp"
#include "Render/RenderContext.hpp"
#include "Render/Logics/RenderAuxFunctions.hpp"

RenderImageLogic::RenderImageLogic() :
    imageScale(1.f),
    texScale(0.f) {
}

RenderImageLogic::~RenderImageLogic() {
}

void RenderImageLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderImageLogic>("RenderImage")) {
        classInfo->addResourceField("image", &RenderImageLogic::ET_setImage);
    }
}

bool RenderImageLogic::init() {
    ET_setGeometry(PrimitiveGeometryType::Sqaure_Tex);
    if(!geom) {
        return false;
    }
    ET_setMaterial("simple_image");
    if(!mat) {
        return false;
    }

    updateScale();

    RenderNode::init();

    ETNode<ETRenderImageLogic>::connect(getEntityId());
    ETNode<ETRenderRect>::connect(getEntityId());
    return true;
}

void RenderImageLogic::onRender(RenderContext& renderCtx) {
    Vec3 scale = Vec3(imageScale.x * texScale.x, imageScale.y * texScale.y, 1.f);
    Mat4 mvp = Render::CalcModelMat(getEntityId(), scale, *geom);
    mvp = renderCtx.proj2dMat * mvp;

    mat->bind();
    mat->setUniformMat4("MVP", mvp);
    mat->setTexture2D("tex", tex->texId);
    geom->draw();
    mat->unbind();
}

void RenderImageLogic::ET_setImage(const char* imageName) {
    if(!imageName || !imageName[0]) {
        tex.reset();
    } else {
        ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createTexture, imageName, ETextureType::RGBA);
    }
    updateScale();
}

Vec2i RenderImageLogic::ET_getOriginalSize() const {
    if(tex) {
        return tex->size;
    }
    return Vec2i(0);
}

void RenderImageLogic::ET_setSize(const Vec2i& newSize) {
    Vec2i size = ET_getOriginalSize();
    imageScale.x = newSize.x / static_cast<float>(size.x);
    imageScale.y = newSize.y / static_cast<float>(size.y);
}

Vec2i RenderImageLogic::ET_getSize() const {
    Vec2i origSize = ET_getOriginalSize();
    Vec2i resSize = Vec2i(static_cast<int>(origSize.x * imageScale.x),
        static_cast<int>(origSize.y * imageScale.y));
    return resSize;
}

void RenderImageLogic::ET_setScale(const Vec2& newScale) {
    imageScale = newScale;
}

void RenderImageLogic::updateScale() {
    if(!tex) {
        return;
    }
    auto geomSize = geom->aabb.getSize();
    texScale.x = static_cast<float>(tex->size.x) / geomSize.x;
    texScale.y = static_cast<float>(tex->size.y) / geomSize.y;
}

bool RenderImageLogic::ET_isVisible() const {
    if(!tex) {
        return false;
    }
    return RenderNode::ET_isVisible();
}
