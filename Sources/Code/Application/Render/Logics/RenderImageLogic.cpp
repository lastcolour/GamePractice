#include "Render/Logics/RenderImageLogic.hpp"
#include "Render/RenderTexture.hpp"
#include "Render/RenderMaterial.hpp"
#include "Render/RenderGeometry.hpp"
#include "Core/JSONNode.hpp"
#include "Game/GameObject.hpp"
#include "Math/MatrixTransform.hpp"

RenderImageLogic::RenderImageLogic() {
}

RenderImageLogic::~RenderImageLogic() {
}

bool RenderImageLogic::serialize(const JSONNode& node) {
    std::string matName;
    node.value("mat", matName);
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
    ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createTexture, imageName);
    if(!tex) {
        ETNode<ETRenderEvents>::disconnect();
    } else {
        ETNode<ETRenderEvents>::connect(getEntityId());
        updateScale();
    }
}

Vec2i RenderImageLogic::ET_getSize() const {
    if(tex) {
        return tex->size;
    }
    return Vec2i(0);
}

void RenderImageLogic::updateScale() {
    auto geomSize = geom->aabb.getSize();
    scale.x = static_cast<float>(tex->size.x) / geomSize.x;
    scale.y = static_cast<float>(tex->size.y) / geomSize.y;
    
    scale.x *= 0.1f;
    scale.y *= 0.1f;
}

Mat4 RenderImageLogic::getModelMat() const {
    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETGameObject::ET_getTransform);
    Mat4 model(1.f);
    const Vec3 center = geom->aabb.getCenter();
    Math::Translate(model, center);
    Math::Rotate(model, tm.quat);
    Math::Translate(model, tm.pt);
    Vec3 resScale = Vec3(scale.x * tm.scale.x, scale.y * tm.scale.y, 1.f);
    Math::Scale(model, resScale);
    return model;
}