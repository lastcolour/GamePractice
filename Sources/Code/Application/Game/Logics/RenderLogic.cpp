#include "Game/Logics/RenderLogic.hpp"
#include "Render/RenderGeometry.hpp"
#include "Render/RenderMaterial.hpp"
#include "Core/JSONNode.hpp"
#include "Math/Transform.hpp"

#include <cassert>

RenderLogic::RenderLogic() :
    params(),
    scale(1.f),
    mat(),
    geom() {
}

RenderLogic::~RenderLogic() {
}

bool RenderLogic::init(const JSONNode& node) {
    std::string matName;
    std::string geomName;
    node.value("mat", matName);
    node.value("geom", geomName);

    geom = ET_SendEventReturn(&ETRender::ET_createGeometry, geomName);
    if(!geom) {
        return false;
    }

    mat = ET_SendEventReturn(&ETRender::ET_createMaterial, matName);
    if(!mat) {
        return false;
    }

    ETNode<ETRenderEvents>::connect(getEntityId());
    ETNode<ETRenderLogic>::connect(getEntityId());

    return true;
}

void RenderLogic::update() {
}

void RenderLogic::ET_onRender(const RenderContext& renderCtx) {
    Mat4 mvp = getModelMat();
    mvp = renderCtx.proj2dMat * mvp;

    mat->bind();
    mat->setUniformMat4("MVP", mvp);
    mat->setUniform4f("color", params.col);
    geom->draw();
    mat->unbind();
}

RenderMaterial& RenderLogic::getMaterial() {
    assert(mat && "Invalid material");
    return *mat.get();
}

Mat4 RenderLogic::getModelMat() const {
    Mat4 model(1.f);
    const Vec3 center = geom->aabb.getCenter();
    Math::Translate(model, center);
    Math::Rotate(model, params.rot, Vec3(0.f, 0.f, 1.f));
    Math::Translate(model, Vec3(params.pt, 1.f));
    Math::Scale(model, Vec3(scale, 1.f));
    return model;
}

void RenderLogic::ET_setRenderParams(const RenderLogicParams& logicParams) {
    params = logicParams;
    Vec3 modelScale = Vec3(params.size, 1.f) / geom->aabb.getSize();
    scale.x = modelScale.x;
    scale.y = modelScale.y;
}