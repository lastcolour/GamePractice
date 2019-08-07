#include "Render/Logics/RenderSimpleLogic.hpp"
#include "Render/RenderGeometry.hpp"
#include "Render/RenderMaterial.hpp"
#include "Core/JSONNode.hpp"
#include "Math/MatrixTransform.hpp"

#include <cassert>

RenderSimpleLogic::RenderSimpleLogic() :
    scale(1.f),
    color(255, 255, 255),
    mat(),
    geom() {
}

RenderSimpleLogic::~RenderSimpleLogic() {
}

bool RenderSimpleLogic::serialize(const JSONNode& node) {
    std::string geomName;
    node.value("geom", geomName);
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, geomName.c_str());
    if(!geom) {
        return false;
    }
    std::string matName;
    node.value("mat", matName);
    ET_SendEventReturn(mat, &ETRenderMaterialManager::ET_createMaterial, matName.c_str());
    if(!mat) {
        return false;
    }
    return true;
}

bool RenderSimpleLogic::init() {
    ETNode<ETRenderEvents>::connect(getEntityId());
    ETNode<ETRenderSimpleLogic>::connect(getEntityId());
    return true;
}

void RenderSimpleLogic::ET_onRender(const RenderContext& renderCtx) {
    Mat4 mvp = getModelMat();
    mvp = renderCtx.proj2dMat * mvp;

    mat->bind();
    mat->setUniformMat4("MVP", mvp);
    mat->setUniform4f("color", color);
    geom->draw();
    mat->unbind();
}

Mat4 RenderSimpleLogic::getModelMat() const {
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

void RenderSimpleLogic::ET_setMaterial(const char* matName) {
    ET_SendEventReturn(mat, &ETRenderMaterialManager::ET_createMaterial, matName);
}

void RenderSimpleLogic::ET_setColor(const ColorB& col) {
    color = col;
}

void RenderSimpleLogic::ET_setSize(const Vec2& sz) {
    Vec3 modelScale = Vec3(sz, 1.f) / geom->aabb.getSize();
    scale.x = modelScale.x;
    scale.y = modelScale.y;
}