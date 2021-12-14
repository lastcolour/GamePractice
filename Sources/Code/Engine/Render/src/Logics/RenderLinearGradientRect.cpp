#include "Logics/RenderLinearGradientRect.hpp"
#include "Math/MatrixTransform.hpp"

RenderLinearGradientRect::RenderLinearGradientRect() :
    DrawCommandProxy(EDrawCmdType::TexturedQuad),
    size(100),
    startCol(255, 255, 255),
    endCol(0, 0, 0),
    isVertical(true) {
}

RenderLinearGradientRect::~RenderLinearGradientRect() {
}

void RenderLinearGradientRect::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderLinearGradientRect>("RenderLinearGradientRect")) {
        classInfo->addBaseClass<DrawCommandProxy>();
        classInfo->addField("size", &RenderLinearGradientRect::size);
        classInfo->addField("startColor", &RenderLinearGradientRect::startCol);
        classInfo->addField("endColor", &RenderLinearGradientRect::endCol);
        classInfo->addField("isVertical", &RenderLinearGradientRect::isVertical);
    }
}

void RenderLinearGradientRect::onInit() {
    auto texQuadCmd = static_cast<DrawTexturedQuadCmd*>(cmd);
    texQuadCmd->imageType = DrawTexturedQuadCmd::EImageCmdType::Gradient;

    Math::AddScale(texQuadCmd->modelMat, Vec3(
        static_cast<float>(size.x), static_cast<float>(size.y), 1.f));

    DrawTexturedQuadCmd::QueueSetTexGradient(*cmd, startCol, endCol, isVertical);

    ETNode<ETRenderRect>::connect(getEntityId());
}

void RenderLinearGradientRect::ET_setSize(const Vec2& newSize) {
    DrawColoredQuadCmd::QueueSizeUpdate(*cmd, size, newSize, EDrawCmdType::TexturedQuad);
    size = newSize;
}

Vec2 RenderLinearGradientRect::ET_getSize() const {
    return size;
}