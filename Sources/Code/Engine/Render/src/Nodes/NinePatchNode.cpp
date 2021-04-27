#include "Nodes/NinePatchNode.hpp"
#include "RenderTexture.hpp"

NinePatchNode::NinePatchNode() :
    horizontal(0.3f),
    vertical(0.3f),
    patchScale(1.f) {
}

NinePatchNode::~NinePatchNode() {
}

void NinePatchNode::onInit() {
    setShader("simple_image");
    setGeometry(PrimitiveGeometryType::NinePatch);
    setBlendingMode(BlendMode{BlendType::SRC_ALPHA, BlendType::ONE_MINUS_SRC_ALPHA});
}

void NinePatchNode::setPatches(float newHorizontal, float newVertical, float newPatchScale) {
    horizontal = newHorizontal;
    vertical = newVertical;
    patchScale = newPatchScale;
}

Vec2 NinePatchNode::getVertCoord() const {
    if(!texObj) {
        return Vec2(0.f);
    }
    Vec2 vertCoord = RenderUtils::GetNinePatchVertexCoord(texObj->getSize(),
        Vec2(size.x * tm.scale.x, size.y * tm.scale.y), Vec2(horizontal, vertical), patchScale);
    return vertCoord;
}

Mat4 NinePatchNode::calcModelMat(const Transform& newTm) {
    return RenderUtils::CalcModelMat(newTm, Vec3(size.x, size.y, 1.f));
}

void NinePatchNode::onRender(RenderContext& ctx) {
    RenderUtils::ApplyTextureInfo(*texObj, texInfo);

    shader->setUniformMat4(UniformType::ModelMat, modelMat);
    shader->setTexture2d(UniformType::Texture, 0, *texObj);
    shader->setUniform4f(UniformType::Color, texInfo.tintColor);

    geom->drawNinePatch(getVertCoord(), Vec2(horizontal, vertical));
}