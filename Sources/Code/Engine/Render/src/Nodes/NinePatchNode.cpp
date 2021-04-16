#include "Nodes/NinePatchNode.hpp"
#include "RenderTexture.hpp"

NinePatchNode::NinePatchNode() :
    tmScale(1.f),
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

Mat4 NinePatchNode::calcModelMat(const Transform& newTm) {
    tmScale = Vec2(newTm.scale.x, newTm.scale.y);
    return RenderUtils::CalcModelMat(newTm, Vec3(size.x, size.y, 1.f));
}

void NinePatchNode::onRender(RenderContext& ctx) {
    Vec2i origSize = texObj->getSize();
    Vec2 texScale = Vec2((size.x * tmScale.x) / origSize.x, (size.y * tmScale.y) / origSize.y);
    float minScale = std::min(texScale.x, texScale.y);
    if(minScale >= 1.f) {
        minScale = 1.f;
    }

    minScale *= patchScale;

    float hPatch = 2.f * std::min(horizontal * minScale / texScale.x, 0.4999f);
    float vPatch = 2.f * std::min(vertical * minScale / texScale.y, 0.4999f);

    RenderUtils::ApplyTextureInfo(*texObj, texInfo);

    shader->setUniformMat4(UniformType::ModelMat, modelMat);
    shader->setTexture2d(UniformType::Texture, 0, *texObj);
    shader->setUniform4f(UniformType::Color, texInfo.tintColor);

    geom->drawNinePatch(Vec2(hPatch, vPatch), Vec2(horizontal, vertical));
}