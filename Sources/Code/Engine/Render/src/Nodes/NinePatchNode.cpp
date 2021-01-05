#include "Nodes/NinePatchNode.hpp"
#include "Logics/RenderAuxFunctions.hpp"

#include <algorithm>

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

void NinePatchNode::onRender(RenderContext& ctx) {
    auto scale = Render::CalcGeomScaleForSize(size, *geom);
    Mat4 modelMat = Render::CalcModelMat(tm, Vec3(scale, 1.f), *geom);

    Vec2i origSize = tex->getSize();
    Vec2 texScale = Vec2((size.x * tm.scale.x) / origSize.x, (size.y * tm.scale.y) / origSize.y);
    float minScale = std::min(texScale.x, texScale.y);
    if(minScale >= 1.f) {
        minScale = 1.f;
    }

    minScale *= patchScale;

    float hPatch = 2.f * std::min(horizontal * minScale / texScale.x, 0.4999f);
    float vPatch = 2.f * std::min(vertical * minScale / texScale.y, 0.4999f);

    shader->setUniformMat4(UniformType::ModelMat, modelMat);
    shader->setTexture2D(UniformType::Texture, *tex);
    shader->setUniform4f(UniformType::Color, tintColor);

    geom->drawNinePatch(Vec2(hPatch, vPatch), Vec2(horizontal, vertical));
}