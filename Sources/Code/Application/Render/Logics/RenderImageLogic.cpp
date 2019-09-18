#include "Render/Logics/RenderImageLogic.hpp"
#include "Render/RenderTexture.hpp"
#include "Render/RenderMaterial.hpp"
#include "Render/RenderGeometry.hpp"
#include "Core/JSONNode.hpp"
#include "Entity/ETEntityInterfaces.hpp"
#include "Math/MatrixTransform.hpp"

RenderImageLogic::RenderImageLogic() :
    imageScale(1.f),
    texScale(0.f) {
}

RenderImageLogic::~RenderImageLogic() {
}

bool RenderImageLogic::serialize(const JSONNode& node) {
    std::string matName;
    node.read("mat", matName);
    ET_SendEventReturn(mat, &ETRenderMaterialManager::ET_createMaterial, matName.c_str());
    if(!mat) {
        return false;
    }
    return true;
}

bool RenderImageLogic::init() {
    if(!mat) {
        return false;
    }
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, "square_tex");
    if(!geom) {
        return false;
    }

    ETNode<ETRenderImageLogic>::connect(getEntityId());
    return true;
}

void RenderImageLogic::ET_onRender(const RenderContext& renderCtx) {
    Mat4 mvp = getModelMat();
    mvp = renderCtx.proj2dMat * mvp;

    mat->bind();
    mat->setUniformMat4("MVP", mvp);
    mat->setTexture2D("tex", tex->texId);
    geom->draw();
    mat->unbind();
}

void RenderImageLogic::ET_setMaterial(const char* matName) {
    ET_SendEventReturn(mat, &ETRenderMaterialManager::ET_createMaterial, matName);
}

void RenderImageLogic::ET_setImage(const char* imageName) {
    if(!imageName || !imageName[0]) {
        tex.reset();
    } else {
        ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createTexture, imageName);
    }
    if(!tex) {
        ETNode<ETRenderEvents>::disconnect();
    } else {
        ETNode<ETRenderEvents>::connect(getEntityId());
        updateScale();
    }
}

Vec2i RenderImageLogic::ET_getOrigSize() const {
    if(tex) {
        return tex->size;
    }
    return Vec2i(0);
}

Vec2i RenderImageLogic::ET_getSize() const {
    Vec2i size = ET_getOrigSize();
    size.x = static_cast<int>(size.x * imageScale.x);
    size.y = static_cast<int>(size.y * imageScale.x);
    return size;
}

void RenderImageLogic::ET_setScale(const Vec2& newScale) {
    imageScale = newScale;
}

void RenderImageLogic::updateScale() {
    auto geomSize = geom->aabb.getSize();
    texScale.x = static_cast<float>(tex->size.x) / geomSize.x;
    texScale.y = static_cast<float>(tex->size.y) / geomSize.y;
}

Mat4 RenderImageLogic::getModelMat() const {
    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    Mat4 model(1.f);
    const Vec3 center = geom->aabb.getCenter();
    Math::Translate(model, center);
    Math::Rotate(model, tm.quat);
    Math::Translate(model, tm.pt);
    Vec3 scale = Vec3(texScale.x * imageScale.x * tm.scale.x,
        texScale.y * imageScale.y * tm.scale.y, 1.f);
    Math::Scale(model, scale);
    return model;
}