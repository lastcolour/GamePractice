#include "Render/Logics/RenderSimpleLogic.hpp"
#include "Render/RenderGeometry.hpp"
#include "Render/RenderMaterial.hpp"
#include "Core/JSONNode.hpp"
#include "Math/MatrixTransform.hpp"

#include <cassert>

RenderSimpleLogic::RenderSimpleLogic() :
    params(),
    scale(1.f),
    mat(),
    geom() {
}

RenderSimpleLogic::~RenderSimpleLogic() {
}

bool RenderSimpleLogic::serialize(const JSONNode& node) {
    std::string geomName;
    node.value("geom", geomName);
    ET_SendEventReturn(geom, &ETRender::ET_createGeometry, geomName);
    if(!geom) {
        return false;
    }
    std::string matName;
    node.value("mat", matName);
    ET_SendEventReturn(mat, &ETRender::ET_createMaterial, matName);
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
    mat->setUniform4f("color", params.col);
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
    Math::Scale(model, Vec3(scale, 1.f));
    return model;
}

void RenderSimpleLogic::ET_setRenderParams(const RenderLogicParams& logicParams) {
    params = logicParams;
    Vec3 modelScale = Vec3(params.size, 1.f) / geom->aabb.getSize();
    scale.x = modelScale.x;
    scale.y = modelScale.y;
}

void RenderSimpleLogic::ET_getRenderParams(RenderLogicParams& logicParams) {
    logicParams = params;
}